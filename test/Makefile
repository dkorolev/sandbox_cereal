CPP=clang++ -std=c++11 -I .
LDFLAGS=-pthread

SRC=$(wildcard test_*.cc)
OBJ=$(SRC:%.cc=build/%.o)
TEST_EXE=build/run_all_tests

.PHONY: all test clean

test: all
	./${TEST_EXE} --gtest_death_test_style=threadsafe

all: build ${TEST_EXE} ${STANDALONE_EXE}

build:
	mkdir -p build

build/%.o: %.cc
	${CPP} -o $@ -c $<

${TEST_EXE}: build/test_cereal.o
	${CPP} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -rf build
