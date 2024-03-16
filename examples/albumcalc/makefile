# Makefile for the example 'albumcalc', part of libarcstk

CXX     := g++
## Replace 'g++' with 'clang++' if you like to use clang
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic
LDFLAGS := "-Wl,--disable-new-dtags,-rpath,../../build/"
## We force to use libarcstk.so from build tree. Check this with
## $ ldd arparse | grep arcs
## or
## $ readelf -d arparse | head -20
## and look for the value of RPATH.

PROJECT := albumcalc

%.o : %.cpp
	$(CXX) $(CFLAGS) -c -I../../include $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(LDFLAGS) $< ../../build/libarcstk.so -lcue -lsndfile -o $@

clean:
	rm -f *.o $(PROJECT)

