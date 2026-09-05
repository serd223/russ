.PHONY:
all: main

CPPC=g++
CPPFLAGS=-Wall, -Wextra $(shell pkg-config sdl3 --cflags)
LIBS=$(shell pkg-config sdl3 --libs)

main: src/*.cpp
	CPPC src/*.cpp $(CPPFLAGS) -o main $(LIBS)

