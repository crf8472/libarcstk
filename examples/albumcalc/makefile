# Makefile for the example 'albumcalc', part of libarcs

CXX     := g++
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic -lsndfile -lcue -larcs

PROJECT := albumcalc

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -O3 $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROJECT)
