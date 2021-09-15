# Makefile

CC=gcc

CPPFLAGS=`pkg-config --cflags sdl2`
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3
LDFLAGS=
LDLIBS= `pkg-config --libs sdl2`

all: main

clean:
	${RM} *.o
	${RM} *.d
	${RM} main

# END