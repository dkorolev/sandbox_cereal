CPP=clang++ -std=c++11 -I .
LDFLAGS=-pthread -lglog -lgflags -static

SRC=$(wildcard test_*.cc)
OBJ=$(SRC:%.cc=build/%.o)
GTEST_OBJ=/usr/src/gtest/libgtest.a /usr/src/gtest/libgtest_main.a
TEST_EXE=build/run_all_tests
STANDALONE_EXE=build/standalone

.PHONY: all test clean

test: all
	./${TEST_EXE} --gtest_death_test_style=threadsafe

all: build ${TEST_EXE} ${STANDALONE_EXE}

build: cereal
	mkdir -p build

build/%.o: %.cc
	${CPP} -o $@ -c $<

${TEST_EXE}: build/test_cereal.o
	${CPP} -o $@ ${OBJ} ${GTEST_OBJ} ${LDFLAGS}

${STANDALONE_EXE}: build/standalone.o
	${CPP} -o $@ $< ${LDFLAGS}

# From git clone git@github.com:USCiLab/cereal.git
# Installed into a directory that requires no extra flags beyond `-I .`.
cereal:
	tar xzf cereal-1.0.0.tar.gz && rm -rf cereal && ln -sf $$PWD/cereal-1.0.0/include/cereal $$PWD/cereal

clean:
	rm -rf build cereal
