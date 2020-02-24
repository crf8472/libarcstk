# Makefile for the example 'albumcalc', part of libarcstk

CXX     := g++
## Replace 'g++' with 'clang++' if you like to use clang
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic

PROJECT := albumcalc

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -O3 $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(CFLAGS) $< -lcue -lsndfile -larcstk -o $@

clean:
	rm -f *.o $(PROJECT)
