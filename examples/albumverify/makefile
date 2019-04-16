# Makefile for the example 'albumverify', part of libarcstk

CXX     := g++
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic -larcs

PROJECT := albumverify

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -O3 $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROJECT)
