.PHONY:
all: main

CPPC=g++
CPPFLAGS=-Wall -Wextra -g $(shell pkg-config sdl3 --cflags)
LIBS=$(shell pkg-config sdl3 --libs)

main: src/*.cpp
	$(CPPC) src/*.cpp $(CPPFLAGS) -o bin/main $(LIBS)

