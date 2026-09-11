.PHONY: all clean main debug teapot car
all: main debug

clean:
	rm -rf bin/main bin/debug

CPPC=g++
CPPFLAGS=-std=c++20 -Wall -Wextra -O3 -I$(shell pwd)/src $(shell pkg-config sdl3 --cflags)
CPPDEBUGFLAGS=-std=c++20 -Wall -Wextra -g -O0 -I$(shell pwd)/src $(shell pkg-config sdl3 --cflags)
LIBS=$(shell pkg-config sdl3 --libs)

SRCS=$(wildcard src/*.cpp)
HEADERS=$(wildcard src/*.hpp)

main: bin/main
bin/main: $(SRCS) $(HEADERS)
	$(CPPC) $(SRCS) $(CPPFLAGS) -o bin/main $(LIBS)

debug: bin/debug
bin/debug: $(SRCS)
	$(CPPC) $(SRCS) $(CPPDEBUGFLAGS) -o bin/debug $(LIBS)

teapot:
	mkdir -p obj
	curl https://raw.githubusercontent.com/alecjacobson/common-3d-test-models/refs/heads/master/data/teapot.obj > ./obj/teapot.obj

car:
	mkdir -p obj
	curl https://raw.githubusercontent.com/alecjacobson/common-3d-test-models/refs/heads/master/data/beetle-alt.obj > ./obj/car.obj
