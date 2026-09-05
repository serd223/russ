.PHONY:
all: bin/main

CPPC=g++
CPPFLAGS=-std=c++20 -Wall -Wextra -g $(shell pkg-config sdl3 --cflags)
LIBS=$(shell pkg-config sdl3 --libs)

SRCS=$(wildcard src/*.cpp)

bin/main: $(SRCS)
	$(CPPC) $(SRCS) $(CPPFLAGS) -o bin/main $(LIBS)


teapot:
	mkdir -p obj
	curl https://raw.githubusercontent.com/alecjacobson/common-3d-test-models/refs/heads/master/data/teapot.obj > ./obj/teapot.obj
