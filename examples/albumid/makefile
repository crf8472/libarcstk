# Makefile for the example 'albumid', part of libarcstk

CXX     := g++
CFLAGS  := -std=c++14 -Wall -Wextra -pedantic -lcue -lsndfile -larcs

PROJECT := albumid

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -O3 $<

$(PROJECT) : $(PROJECT).o
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROJECT)
