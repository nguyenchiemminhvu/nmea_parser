- [NMEA Generator — Developer Guide](#nmea-generator--developer-guide)
  - [1. Module Structure](#1-module-structure)
  - [2. Dependency Boundaries](#2-dependency-boundaries)
  - [3. API Usage Examples](#3-api-usage-examples)
    - [3.1 High-level — from shared buffers](#31-high-level--from-shared-buffers)
    - [3.2 With timing data (high-accuracy ZDA)](#32-with-timing-data-high-accuracy-zda)
    - [3.3 Low-level — manual DTO population](#33-low-level--manual-dto-population)
    - [3.4 GSV only for specific constellations](#34-gsv-only-for-specific-constellations)
    - [3.5 Reconfigure without reallocating](#35-reconfigure-without-reallocating)
  - [4. How to Add a New Sentence Type](#4-how-to-add-a-new-sentence-type)

# NMEA Generator — Developer Guide

## 1. Module Structure

```
include/generator/              ← Public API (callers include these)
  i_nmea_checksum.h             Checksum interface
  i_nmea_formatter.h            Field encoding interface
  i_nmea_sentence_builder.h     Per-sentence builder interface
  nmea_generator_config.h       Configuration struct
  nmea_sentence_inputs.h        Input DTOs for each sentence type
  nmea_generator_facade.h       Main entry point (pImpl)

src/generator/                  ← Implementation (not included by callers)
  nmea_checksum_impl.h          Concrete XOR checksum (header-only)
  nmea_formatter.h / .cpp       Concrete field encoder
  shared_buffer_mapper.h        gnss:: struct → DTO mapping (header-only)
  nmea_generator_facade.cpp     Facade implementation + pImpl struct
  builders/
    sentence_builder_base.h     CRTP-free base: assemble(), talker, enabled
    gga_builder.h / .cpp
    rmc_builder.h / .cpp
    gsa_builder.h / .cpp
    gsv_builder.h / .cpp        build_all() returns vector<string>
    vtg_builder.h / .cpp
    gll_builder.h / .cpp
    zda_builder.h / .cpp
```

---

## 2. Dependency Boundaries

```
nmea_generator_facade.h
  depends on:
    nmea_generator_config.h    (config struct — no external deps)
    nmea_sentence_inputs.h     (DTOs — <cstdint> <string> <vector> only)
    gnss:: forward declarations (location_data, satellites_data, timing_data, dop_data)

Callers must NOT include anything from src/generator/ directly.
The builder headers are private; only nmea_generator_facade.cpp uses them.
```

---

## 3. API Usage Examples

### 3.1 High-level — from shared buffers

```cpp
#include "generator/nmea_generator_facade.h"
using namespace nmea::generator;

// Construct once (e.g. at Location Service startup)
nmea_generator_config cfg;
cfg.talker_id  = "GN";
cfg.enable_gsa = true;   // omit when DOP data is never available
nmea_generator_facade gen(cfg);

// Called from HP-commit callback (parser worker thread)
void on_gnss_epoch(const gnss::location_data& loc,
                   const gnss::satellites_data& sats,
                   const gnss::dop_data& dop)
{
    // Optional: populate DOP if UBX-NAV-DOP handler is installed
    // dop.hdop = ubx_db->get_dop().hdop / 100.0f; // when available

    auto sentences = gen.generate(loc, sats, dop);
    for (const auto& s : sentences)
        output_channel.write(s.data(), s.size());
}
```

### 3.2 With timing data (high-accuracy ZDA)

```cpp
auto sentences = gen.generate(loc, sats, timing, dop);
```

### 3.3 Low-level — manual DTO population

```cpp
gga_input gga;
gga.hour = 12; gga.minute = 30; gga.second = 0; gga.millisecond = 0;
gga.latitude_deg   = 48.8566;
gga.longitude_deg  = 2.3522;
gga.fix_quality    = 1u;
gga.num_satellites = 8u;
gga.hdop           = 1.2;
gga.altitude_msl_m = 35.0;
gga.geoid_sep_m    = 48.0;
gga.valid          = true;

std::string sentence = gen.generate_gga(gga);
// → "$GNGGA,123000.00,4851.39600,N,00221.13200,E,1,08,1.2,35.0,M,48.0,M,,*hh\r\n"
```

### 3.4 GSV only for specific constellations

```cpp
nmea_generator_config cfg;
cfg.enable_gga = cfg.enable_rmc = cfg.enable_gsa = false;
cfg.enable_vtg = cfg.enable_gll = cfg.enable_zda = false;
cfg.enable_gsv = true;
cfg.enable_gsv_glonass = false;  // suppress GLGSV
nmea_generator_facade gen(cfg);
auto gsv_sentences = gen.generate_gsv_all(sats);
```

### 3.5 Reconfigure without reallocating

```cpp
nmea_generator_config new_cfg = gen.config();
new_cfg.enable_gll = false;
gen.configure(new_cfg);  // rebuilds builders in-place
```

---

## 4. How to Add a New Sentence Type

Example: adding GST (Pseudo Range Error Statistics).

**Step 1 — Add input DTO** in `include/generator/nmea_sentence_inputs.h`:
```cpp
struct gst_input {
    uint8_t hour = 0u; uint8_t minute = 0u; uint8_t second = 0u;
    uint32_t millisecond = 0u;
    double rms_residual = 0.0;
    double error_lat_m  = 0.0;
    double error_lon_m  = 0.0;
    double error_alt_m  = 0.0;
    bool valid = false;
};
```

**Step 2 — Add enable flag** in `include/generator/nmea_generator_config.h`:
```cpp
bool enable_gst = false;   // disabled by default — not all receivers produce this
```

**Step 3 — Create builder** in `src/generator/builders/gst_builder.h/.cpp`:
```cpp
class gst_builder : public sentence_builder_base {
public:
    gst_builder(const std::string& t, const i_nmea_formatter& f,
                const i_nmea_checksum& c, bool enabled = false);
    void prepare(const gst_input& in);
    const char* sentence_type() const override { return "GST"; }
    std::string build() const override;
private:
    gst_input input_;
    bool      input_valid_ = false;
};
```

**Step 4 — Wire into the facade pImpl** in `src/generator/nmea_generator_facade.cpp`:
```cpp
// In impl struct:
std::unique_ptr<gst_builder> gst;
// In rebuild_builders():
gst.reset(new gst_builder(t, formatter, checksum, config.enable_gst));
```

**Step 5 — Add mapper** in `src/generator/shared_buffer_mapper.h`:
```cpp
inline gst_input to_gst(const gnss::location_data& loc) { ... }
```

**Step 6 — Expose in facade** in `include/generator/nmea_generator_facade.h`:
```cpp
std::string generate_gst(const gst_input& input) const;
```
