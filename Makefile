# Build the compiler:            make
# Debug build with sanitizers:   make debug
# Run the test suite:            make test
# Compare against an oracle:     make fuzz
# Compile and run one program:   make run PROGRAM=tests/programs/examples/sieve.imp
#
# Tools and flags can be overridden, e.g. `make CXX=clang++ BISON=/opt/bison/bin/bison`.

CXX      ?= g++
BISON    ?= bison
FLEX     ?= flex
PYTHON   ?= python3
VM       ?= vm/vm

CXXFLAGS ?= -O2
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS  ?=

BUILD    ?= build/release
TARGET   := $(BUILD)/kompilator
GENDIR   := $(BUILD)/generated

SOURCES  := $(shell find src -mindepth 2 -name '*.cpp')
OBJECTS  := $(SOURCES:src/%.cpp=$(BUILD)/obj/%.o) $(GENDIR)/Parser.o $(GENDIR)/Lexer.o
CPPFLAGS := -Isrc -I$(GENDIR) -MMD -MP

.PHONY: all debug test fuzz run clean help

all: $(TARGET)
	@ln -sfn $(abspath $(TARGET)) build/kompilator

debug:
	@$(MAKE) --no-print-directory BUILD=build/debug \
		CXXFLAGS="-O0 -g -fsanitize=address,undefined" LDFLAGS="-fsanitize=address,undefined"

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# --- generated parser and lexer -------------------------------------------------------

$(GENDIR)/Parser.cpp: src/frontend/Parser.y
	@mkdir -p $(@D)
	$(BISON) -d -o $@ $<

$(GENDIR)/Parser.hpp: $(GENDIR)/Parser.cpp ;

$(GENDIR)/Lexer.cpp: src/frontend/Lexer.l
	@mkdir -p $(@D)
	$(FLEX) -o $@ $<

$(GENDIR)/%.o: $(GENDIR)/%.cpp $(GENDIR)/Parser.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

# --- hand-written sources ----------------------------------------------------------------

$(BUILD)/obj/%.o: src/%.cpp | $(GENDIR)/Parser.hpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(OBJECTS:.o=.d)

# --- tasks -------------------------------------------------------------------------------

test: all
	@tests/run_tests.sh $(TARGET) $(VM)

fuzz: all
	@$(PYTHON) tests/fuzz.py --compiler $(TARGET) --vm $(VM)

run: all
	@test -n "$(PROGRAM)" || { echo "usage: make run PROGRAM=<file.imp>"; exit 1; }
	@mkdir -p $(BUILD)/run
	$(TARGET) $(PROGRAM) $(BUILD)/run/program.mr
	$(VM) $(BUILD)/run/program.mr

clean:
	rm -rf build

help:
	@sed -n 's/^# \{0,1\}//p' Makefile | sed -n '1,/^$$/p'
