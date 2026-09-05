.PHONY:
all: main

CPPC=g++
CPPFLAGS=-Wall -Wextra -g $(shell pkg-config sdl3 --cflags)
LIBS=$(shell pkg-config sdl3 --libs)

main: src/*.cpp
	$(CPPC) src/*.cpp $(CPPFLAGS) -o bin/main $(LIBS)


teapot:
	mkdir -p obj
	curl https://raw.githubusercontent.com/alecjacobson/common-3d-test-models/refs/heads/master/data/teapot.obj > ./obj/teapot.obj
