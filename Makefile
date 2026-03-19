# ─────────────────────────────────────────────────────────────────────────────
# Makefile — nmea_parser library
# Target: Linux / macOS, C++14
# ─────────────────────────────────────────────────────────────────────────────

CXX      ?= g++
CXXFLAGS  = -std=c++14 -Wall -Wextra -Wpedantic \
            -Iinclude \
            -Isrc
AR       ?= ar
ARFLAGS   = rcs

# Link with pthread (required for std::shared_timed_mutex on Linux).
# On macOS pthreads is bundled; the flag is accepted but not strictly needed.
LDFLAGS   = -lpthread

# ── Library sources ───────────────────────────────────────────────────────────

LIB_SRCS = \
    src/nmea_sentence_registry.cpp       \
    src/nmea_dispatcher.cpp              \
    src/nmea_parser.cpp                  \
    src/parsers/gga_parser.cpp           \
    src/parsers/rmc_parser.cpp           \
    src/parsers/gsa_parser.cpp           \
    src/parsers/gsv_parser.cpp           \
    src/parsers/gll_parser.cpp           \
    src/parsers/vtg_parser.cpp           \
    src/parsers/gbs_parser.cpp           \
    src/parsers/gns_parser.cpp           \
    src/parsers/gst_parser.cpp           \
    src/parsers/zda_parser.cpp           \
    src/parsers/dtm_parser.cpp           \
    src/database/nmea_snapshot.cpp       \
    src/database/nmea_database.cpp       \
    src/database/nmea_database_adapter.cpp \
    src/database/handlers/db_gga_handler.cpp \
    src/database/handlers/db_rmc_handler.cpp \
    src/database/handlers/db_gsa_handler.cpp \
    src/database/handlers/db_gsv_handler.cpp \
    src/database/handlers/db_vtg_handler.cpp

LIB_OBJS = $(LIB_SRCS:.cpp=.o)
LIB_NAME = libnmea_parser.a

# ── Test sources ──────────────────────────────────────────────────────────────

TEST_SRCS = \
    tests/main.cpp                   \
    tests/test_checksum.cpp          \
    tests/test_parser_stream.cpp     \
    tests/test_parsers_gga.cpp       \
    tests/test_parsers_rmc.cpp       \
    tests/test_parsers_gsa.cpp       \
    tests/test_parsers_gsv.cpp       \
    tests/test_parsers_gll.cpp       \
    tests/test_parsers_vtg.cpp       \
    tests/test_parsers_gbs.cpp       \
    tests/test_parsers_gst.cpp       \
    tests/test_parsers_zda.cpp       \
    tests/test_database_gga.cpp      \
    tests/test_database_epoch.cpp

TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_BIN  = nmea_test_runner

# ── Example sources ───────────────────────────────────────────────────────────

EXAMPLE_SRC = examples/basic_parse_demo.cpp
EXAMPLE_BIN = basic_parse_demo

# ── Default target ────────────────────────────────────────────────────────────

.PHONY: all clean run_tests example

all: $(LIB_NAME)

# ── Build library ─────────────────────────────────────────────────────────────

$(LIB_NAME): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^
	@echo "Built $@"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Build and run tests ───────────────────────────────────────────────────────

$(TEST_BIN): $(LIB_NAME) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) -L. -lnmea_parser $(LDFLAGS) -o $@

run_tests: $(TEST_BIN)
	./$(TEST_BIN)


# ── Build and run example ─────────────────────────────────────────────────────

$(EXAMPLE_BIN): $(LIB_NAME) $(EXAMPLE_SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) $(EXAMPLE_SRC:.cpp=.o) -L. -lnmea_parser $(LDFLAGS) -o $@

example: $(EXAMPLE_BIN)
	./$(EXAMPLE_BIN)

# ── Clean ─────────────────────────────────────────────────────────────────────

clean:
	rm -f $(LIB_OBJS) $(TEST_OBJS) $(EXAMPLE_SRC:.cpp=.o) \
	      $(LIB_NAME) $(TEST_BIN) $(EXAMPLE_BIN) run_tests
