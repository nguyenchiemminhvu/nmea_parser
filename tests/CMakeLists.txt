cmake_minimum_required(VERSION 3.14)
project(nmea_parser_tests VERSION 1.0.0 LANGUAGES CXX)

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

# ── Tests ─────────────────────────────────────────────────────────────────────

enable_testing()

add_executable(nmea_test_runner
    main.cpp
    test_checksum.cpp
    test_parser_stream.cpp
    test_parsers_gga.cpp
    test_parsers_rmc.cpp
    test_parsers_gsa.cpp
    test_parsers_gsv.cpp
    test_parsers_gll.cpp
    test_parsers_vtg.cpp
    test_parsers_gbs.cpp
    test_parsers_gst.cpp
    test_parsers_zda.cpp
    test_database_gga.cpp
    test_database_epoch.cpp
)

target_link_libraries(nmea_test_runner PRIVATE nmea_parser)

add_test(NAME nmea_tests COMMAND nmea_test_runner)
