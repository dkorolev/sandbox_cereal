CPP=clang++ -std=c++11 -I .
LDFLAGS=-pthread

SRC=$(wildcard test_*.cc)
OBJ=$(SRC:%.cc=build/%.o)
STANDALONE_EXE=build/standalone

.PHONY: all test clean

all: build ${STANDALONE_EXE}

build:
	mkdir -p build

build/%.o: %.cc
	${CPP} -o $@ -c $<

${STANDALONE_EXE}: build/standalone.o
	${CPP} -o $@ $< ${LDFLAGS}

clean:
	rm -rf build
