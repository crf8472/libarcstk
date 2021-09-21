# Makefile for the example 'albumverify', part of libarcstk

CXX     := g++
## Replace 'g++' with 'clang++' if you like to use clang
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic

PROJECT := albumverify

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -O3 $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(CFLAGS) $< -o $@ ../../build/libarcstk.so

clean:
	rm -f *.o $(PROJECT)
