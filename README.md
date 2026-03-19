# nmea_parser

A production-grade, header-clean C++14 library for parsing NMEA 0183 sentences from a streaming byte source. The API is designed for real-time applications: no heap allocation in hot paths, incremental byte feeding, and a multi-subscriber registry for clean separation of concerns between user code and the optional database layer.

---

## Features

- **Incremental byte-stream ingestion** — feed any chunk size down to one byte at a time.
- **Full framing state machine** — detects `$`…`*HH\r\n` structure, computes and validates XOR checksums.
- **Multi-subscriber dispatch** — multiple callbacks may be registered for the same sentence type independently.
- **Typed sentence structs** — decoded data returned in clean, zero-heap field structs.
- **Optional database/epoch layer** — flat double store + snapshot + commit policy, mirroring `ubx_parser`'s database layer.
- **Error reporting callbacks** — checksum mismatch, sentence-too-long, unexpected `$`, malformed fields.
- **No exceptions in hot paths** — all parse helpers return `bool`.
- **C++14, no external dependencies** — only stdlib + pthreads.

---

## Supported sentences

| Sentence | Description |
|----------|-------------|
| GGA | Global Positioning System Fix Data |
| RMC | Recommended Minimum Specific GNSS Data |
| GSA | GNSS DOP and Active Satellites |
| GSV | GNSS Satellites in View (multi-part) |
| GLL | Geographic Position – Latitude/Longitude |
| VTG | Course Over Ground and Ground Speed |
| GBS | GNSS Satellite Fault Detection |
| GNS | GNSS Fix Data |
| GST | GNSS Pseudorange Error Statistics |
| ZDA | Time & Date |
| DTM | Datum Reference |

Multi-talker prefixes (`GP`, `GN`, `GA`, `GB`, `GL`, `GQ`, `P`) are all handled transparently.  The talker string is preserved on every decoded struct.

---

## Directory layout

```
nmea_parser/
├── include/
│   ├── nmea_types.h                  — constants / talker / sentence-type strings
│   ├── nmea_errors.h                 — parse_error_code, parse_error_info
│   ├── nmea_parser.h                 — main entry point (nmea_parser class)
│   ├── nmea_sentence_registry.h      — per-type parser list
│   ├── nmea_dispatcher.h             — routes tokenised sentence to parsers
│   ├── sentences/                    — one header per sentence type
│   │   ├── nmea_sentence.h           — raw tokenised sentence
│   │   ├── nmea_gga.h  nmea_rmc.h  …
│   ├── parsers/                      — one parser class per sentence type
│   │   ├── i_sentence_parser.h
│   │   ├── gga_parser.h  rmc_parser.h  …
│   └── database/
│       ├── nmea_data_fields.h        — flat field enum (data_field)
│       ├── nmea_field_store.h        — double[DATA_NUM] + valid[] store
│       ├── nmea_snapshot.h           — immutable value-copy with commit_kind
│       ├── nmea_database.h           — thread-safe store owner
│       ├── nmea_database_adapter.h   — wires DB handlers into the parser registry
│       ├── i_update_policy.h         — commit_kind policy interface
│       ├── default_update_policy.h
│       └── handlers/                 — one handler per DB-tracked message
│           ├── i_sentence_handler.h
│           ├── db_gga_handler.h  db_rmc_handler.h  …
├── src/
│   ├── internal/                     — private helpers (not installed)
│   │   ├── parser_state.h            — state machine enum + context
│   │   ├── sentence_tokenizer.h      — splits raw buffer into nmea_sentence
│   │   ├── checksum.h                — XOR + hex-byte helpers
│   │   └── field_parser.h            — typed parse_ helpers
│   ├── nmea_parser.cpp               — state machine implementation
│   ├── nmea_dispatcher.cpp
│   ├── nmea_sentence_registry.cpp
│   ├── parsers/                      — 11 sentence parser .cpp files
│   └── database/
│       ├── nmea_database.cpp
│       ├── nmea_snapshot.cpp
│       ├── nmea_database_adapter.cpp
│       └── handlers/                 — 5 DB handler .cpp files
├── tests/                            — 50 unit tests (no external framework)
├── examples/
│   └── basic_parse_demo.cpp
├── Makefile
└── LICENSE                           — MIT
```

---

## Build

Requires: g++ (or clang++) with C++14 support, GNU make, pthreads.

```bash
# Build static library only
make

# Build + run all tests
make run_tests

# Build + run the integration demo
make example

# Remove all build artefacts
make clean
```

---

## Quick start — parser only

Register typed callbacks directly on the parser. No database layer needed.

```cpp
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "parsers/gga_parser.h"
#include "parsers/rmc_parser.h"

using namespace nmea::parser;

nmea_sentence_registry reg;

reg.register_parser(std::unique_ptr<gga_parser>(
    new gga_parser([](const nmea_gga& g) {
        printf("lat=%.6f  lon=%.6f  fix=%d  sats=%d\n",
               g.latitude, g.longitude,
               (int)g.fix_quality, g.num_satellites);
    })));

reg.register_parser(std::unique_ptr<rmc_parser>(
    new rmc_parser([](const nmea_rmc& r) {
        printf("speed=%.3f kn  course=%.1f deg\n",
               r.speed_knots, r.course_true);
    })));

nmea_parser parser(std::move(reg));

// Optional: error reporting
parser.set_error_callback([](const parse_error_info& e) {
    fprintf(stderr, "NMEA error [%s]: %s\n",
            e.sentence_type.c_str(), e.description.c_str());
});

// Feed data incrementally — any chunk size is fine
while (serial_port.read(buf, len))
    parser.feed(buf, len);           // const char*, size_t overload
```

---

## Quick start — parser + database

The database layer accumulates fields from multiple sentence types into a flat store and fires commit callbacks when a configured policy triggers.

```cpp
#include "nmea_parser.h"
#include "nmea_sentence_registry.h"
#include "database/nmea_database.h"
#include "database/nmea_database_adapter.h"
#include "database/nmea_data_fields.h"
#include "database/handlers/db_gga_handler.h"
#include "database/handlers/db_rmc_handler.h"
#include "database/handlers/db_gsv_handler.h"

using namespace nmea::parser;
using namespace nmea::database;

// 1. Create database
auto db = std::make_shared<nmea_database>();  // uses default_update_policy

// 2. Register commit callbacks
db->set_commit_callback(commit_kind::high_priority, [](const nmea_snapshot& snap) {
    // Fired by GGA or RMC (high-priority sentences)
    double lat = 0.0, lon = 0.0;
    snap.get(NMEA_GGA_LATITUDE,  lat);
    snap.get(NMEA_GGA_LONGITUDE, lon);
    printf("[epoch] lat=%.6f  lon=%.6f\n", lat, lon);
});

db->set_commit_callback(commit_kind::low_priority, [](const nmea_snapshot& snap) {
    // Fired when a complete GSV set is received
    int nsats = 0;
    snap.get(NMEA_GSV_NUM_SATS_IN_VIEW, nsats);
    printf("[sky] sats_in_view=%d\n", nsats);
});

// 3. Wire handlers into the registry
nmea_sentence_registry reg;
nmea_database_adapter  adapter(db);

adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gga_handler()));
adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_rmc_handler()));
adapter.add_handler(std::unique_ptr<i_sentence_handler>(new db_gsv_handler()));
adapter.register_with_parser(reg);

// 4. Create parser — adapter must outlive the parser
nmea_parser parser(std::move(reg));

// 5. Feed bytes
while (serial_port.read(buf, len))
    parser.feed(buf, len);

// 6. Thread-safe ad-hoc snapshot at any time
nmea_snapshot snap = db->snapshot();
double alt = 0.0;
if (snap.get(NMEA_GGA_ALTITUDE_MSL, alt))
    printf("altitude = %.1f m\n", alt);
```

---

## Framing protocol

```
$[talker][type],[f0],[f1],...,[fN]*[CS_HI][CS_LO]\r\n
```

| Element | Details |
|---------|---------|
| `$` | Sentence start |
| talker | 1–2 char prefix: `GP`, `GN`, `GA`, `GB`, `GL`, `GQ`, `P` (proprietary) |
| type | Sentence identifier: `GGA`, `RMC`, etc. |
| fields | Comma-separated; may be empty |
| `*HHHH` | Optional XOR checksum over bytes between `$` and `*` |
| `\r\n` | Sentence terminator (`\n`-only also accepted) |

Max sentence length: 82 chars (buffer 256 to accommodate proprietary sentences).

### State machine

```
┌──────────────┐  $   ┌────────────────────┐
│ WAIT_DOLLAR  │ ───► │ ACCUMULATE_SENTENCE │
└──────────────┘      └────────────────────┘
                               │ *
                               ▼
                     ┌──────────────────┐
                     │ WAIT_CHECKSUM_HI │
                     └──────────────────┘
                               │ hex
                               ▼
                     ┌──────────────────┐
                     │ WAIT_CHECKSUM_LO │
                     └──────────────────┘
                               │ hex → dispatch
                               ▼
                           WAIT_LF
```

---

## Error handling

Register a callback to receive structured error reports:

```cpp
parser.set_error_callback([](const parse_error_info& e) {
    // e.code          — parse_error_code enum value
    // e.talker        — talker prefix (may be empty)
    // e.sentence_type — type string (may be empty if framing failed)
    // e.description   — human-readable message
});
```

Error codes:

| Code | Triggered when |
|------|----------------|
| `checksum_mismatch` | Computed ≠ received checksum |
| `sentence_too_long` | Buffer overflow before `*` or `\r\n` |
| `unexpected_start_in_sentence` | `$` received while accumulating |
| `malformed_fields` | Tokeniser cannot split talker+type |
| `unknown_sentence_type` | No parser registered for the type (opt-in) |

Unknown sentence type reporting is off by default. Enable with:
```cpp
parser.set_report_unknown_sentences(true);
```

---

## Data layer model

The database layer stores every measurable as `double` in a flat array indexed by `data_field` enum values.  Thread safety uses `std::shared_timed_mutex`: writes are exclusive, reads (snapshot) are shared.

### Commit policy

The default policy (`default_update_policy`) fires:

| Sentence | Commit kind |
|----------|-------------|
| GGA | `high_priority` |
| RMC | `high_priority` |
| GSV (last part) | `low_priority` |
| all others | `none` (writes to store, no callback) |

Supply a custom policy via the `nmea_database` constructor:

```cpp
struct my_policy : public nmea::database::i_update_policy {
    commit_kind should_commit(msg_type t) const override {
        if (t == MSG_NMEA_GGA || t == MSG_NMEA_RMC) return commit_kind::high_priority;
        if (t == MSG_NMEA_GSV || t == MSG_NMEA_VTG) return commit_kind::low_priority;
        return commit_kind::none;
    }
};

auto db = std::make_shared<nmea_database>(
    std::unique_ptr<i_update_policy>(new my_policy()));
```

### Snapshot

`nmea_snapshot` is an immutable value copy of the entire store at commit time. It carries:
- `kind()` — `commit_kind` that triggered the snapshot
- `msg_mask()` — bitmask of `msg_type` bits that have been written since the last snapshot of the same kind
- `get<T>(data_field, T& out) → bool` — type-safe field accessor

---

## Extending with a new sentence type

1. Add a struct `nmea_xyz` in `include/sentences/nmea_xyz.h`.
2. Add `xyz_parser` in `include/parsers/xyz_parser.h` + `src/parsers/xyz_parser.cpp`.
   - Override `sentence_type()` to return `"XYZ"`.
   - Implement `parse(const nmea_sentence&)` using `internal::field_parser` helpers.
3. (Optional) Add a `db_xyz_handler` in `include/database/handlers/` / `src/database/handlers/` following the existing handler pattern.
4. Add an enum entry in `msg_type` and field slots in `data_field`.
5. Add the new `.cpp` files to `LIB_SRCS` in `Makefile`.

---

## Design notes — nmea_parser vs ubx_parser

| Aspect | ubx_parser | nmea_parser |
|--------|-----------|-------------|
| Protocol framing | Binary: sync, class, ID, len, payload, CK_A/CK_B | ASCII: `$`…`*HHHH\r\n`, XOR checksum |
| Registry model | Single decoder per message type | **Multiple subscribers per sentence type** (vector) |
| Byte-stream API | `feed(vector<uint8_t>)` | `feed(vector<uint8_t>)` **+** `feed(const char*, size_t)` |
| Multi-part messages | UBX framing is single-packet | GSV multi-part accumulation in `db_gsv_handler` |
| Field parsing | Struct-of-structs from binary offsets | Typed helpers: `parse_double`, `parse_latlon`, `parse_utc_time`, … |
| Namespace | `ubx::parser` / `ubx::database` | `nmea::parser` / `nmea::database` |
| Thread safety | Same (shared_timed_mutex on DB) | Same |
| Test framework | Hand-rolled pass/fail macros | Same |

The multi-subscriber registry is the headline improvement: user code and the DB adapter can independently subscribe to `GGA` (for example) without either knowing about the other.

---

## License

MIT License — Copyright (c) 2026 nguyenchiemminhvu.  See [LICENSE](LICENSE).
