cmake_minimum_required(VERSION 3.14)
project(nmea_parser_examples VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

# ── Library ───────────────────────────────────────────────────────────────────

set(NMEA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(nmea_parser STATIC
    ${NMEA_ROOT}/src/nmea_sentence_registry.cpp
    ${NMEA_ROOT}/src/nmea_dispatcher.cpp
    ${NMEA_ROOT}/src/nmea_parser.cpp
    ${NMEA_ROOT}/src/parsers/gga_parser.cpp
    ${NMEA_ROOT}/src/parsers/rmc_parser.cpp
    ${NMEA_ROOT}/src/parsers/gsa_parser.cpp
    ${NMEA_ROOT}/src/parsers/gsv_parser.cpp
    ${NMEA_ROOT}/src/parsers/gll_parser.cpp
    ${NMEA_ROOT}/src/parsers/vtg_parser.cpp
    ${NMEA_ROOT}/src/parsers/gbs_parser.cpp
    ${NMEA_ROOT}/src/parsers/gns_parser.cpp
    ${NMEA_ROOT}/src/parsers/gst_parser.cpp
    ${NMEA_ROOT}/src/parsers/zda_parser.cpp
    ${NMEA_ROOT}/src/parsers/dtm_parser.cpp
    ${NMEA_ROOT}/src/database/nmea_snapshot.cpp
    ${NMEA_ROOT}/src/database/nmea_database.cpp
    ${NMEA_ROOT}/src/database/nmea_database_adapter.cpp
    ${NMEA_ROOT}/src/database/handlers/db_gga_handler.cpp
    ${NMEA_ROOT}/src/database/handlers/db_rmc_handler.cpp
    ${NMEA_ROOT}/src/database/handlers/db_gsa_handler.cpp
    ${NMEA_ROOT}/src/database/handlers/db_gsv_handler.cpp
    ${NMEA_ROOT}/src/database/handlers/db_vtg_handler.cpp
)

target_include_directories(nmea_parser PUBLIC
    ${NMEA_ROOT}/include
    ${NMEA_ROOT}/src
)

find_package(Threads REQUIRED)
target_link_libraries(nmea_parser PUBLIC Threads::Threads)

# ── Generator library ─────────────────────────────────────────────────────────

add_library(nmea_generator STATIC
    ${NMEA_ROOT}/src/generator/nmea_generator_facade.cpp
    ${NMEA_ROOT}/src/generator/nmea_formatter.cpp
    ${NMEA_ROOT}/src/generator/builders/gga_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/rmc_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/gsa_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/gsv_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/vtg_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/gll_builder.cpp
    ${NMEA_ROOT}/src/generator/builders/zda_builder.cpp
)

# Generator needs:
#   • public headers  (include/)
#   • private impl headers (src/, src/generator/)
# It also reuses internal::compute_checksum from nmea_parser, so link it.
target_include_directories(nmea_generator PUBLIC
    ${NMEA_ROOT}/include
    ${NMEA_ROOT}/src
    ${NMEA_ROOT}/src/generator
)

target_link_libraries(nmea_generator PUBLIC nmea_parser)

# ── Examples ──────────────────────────────────────────────────────────────────

add_executable(basic_parse_demo
    basic_parse_demo.cpp
)

target_link_libraries(basic_parse_demo PRIVATE nmea_parser)

add_executable(basic_nmea_generating
    basic_nmea_generating.cpp
)

target_link_libraries(basic_nmea_generating PRIVATE nmea_generator)
