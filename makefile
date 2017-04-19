#declare the variable
CC = g++

CFLAGS=-c -Wall

all: a3search

bwtsearch: a3search.o
	$(CC)  a3search.o -o a3search

bwtsearch.o:
	$(CC) $(CFLAGS) a3search.cpp

.PHONY: clean
clean:
	rm -rf *o a3search
