CPP=g++ -std=c++11 -I .
LDFLAGS=-pthread -lglog -lgflags -static

SRC=$(wildcard test_*.cc)
OBJ=$(SRC:%.cc=build/%.o)
GTEST_OBJ=/usr/src/gtest/libgtest.a /usr/src/gtest/libgtest_main.a
EXE=build/run_all_tests

.PHONY: all test clean

test: all
	./${EXE} --gtest_death_test_style=threadsafe

all: build ${EXE}

build: cereal
	mkdir -p build

build/%.o: %.cc
	${CPP} -o $@ -c $<

${EXE}: ${OBJ}
	${CPP} -o $@ ${OBJ} ${GTEST_OBJ} ${LDFLAGS}

# From git clone git@github.com:USCiLab/cereal.git
# Installed into a directory that requires no extra flags beyond `-I .`.
cereal:
	tar xjf cereal.tar.bz2

clean:
	rm -rf build cereal
