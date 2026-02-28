# Makefile for the example 'dbarparse', part of libarcstk

CXX     := g++
## Replace 'g++' with 'clang++' if you like to use clang
CFLAGS  := -std=c++17 -Wall -Wextra -pedantic
LDFLAGS := "-Wl,--disable-new-dtags,-rpath,../../build/"
## We force to use libarcstk.so from build tree. Check this with
## $ ldd dbarparse | grep arcs
## or
## $ readelf -d dbarparse | head -20
## and look for the value of RPATH.

PROJECT := dbarparse

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -I../../build/include/arcstk $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(LDFLAGS) $< ../../build/libarcstk.so -o $@

clean:
	rm -f *.o $(PROJECT)

