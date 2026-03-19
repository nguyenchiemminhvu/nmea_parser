// MIT License
//
// Copyright (c) 2026 nguyenchiemminhvu
//
// tests/main.cpp — Test runner entry point

#include <cstdio>

// Forward declarations for all test suites
void run_checksum_tests();
void run_parser_stream_tests();
void run_parsers_gga_tests();
void run_parsers_rmc_tests();
void run_parsers_gsa_tests();
void run_parsers_gsv_tests();
void run_parsers_gll_tests();
void run_parsers_vtg_tests();
void run_parsers_gbs_tests();
void run_parsers_gst_tests();
void run_parsers_zda_tests();
void run_database_gga_tests();
void run_database_epoch_tests();

int main()
{
    std::printf("=== nmea_parser test suite ===\n\n");

    run_checksum_tests();
    run_parser_stream_tests();
    run_parsers_gga_tests();
    run_parsers_rmc_tests();
    run_parsers_gsa_tests();
    run_parsers_gsv_tests();
    run_parsers_gll_tests();
    run_parsers_vtg_tests();
    run_parsers_gbs_tests();
    run_parsers_gst_tests();
    run_parsers_zda_tests();
    run_database_gga_tests();
    run_database_epoch_tests();

    std::printf("\n=== done ===\n");
    return 0;
}
