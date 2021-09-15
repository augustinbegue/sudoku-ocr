# Makefile

CC=gcc

CPPFLAGS=`pkg-config --cflags sdl2` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3
LDFLAGS=
LDLIBS= `pkg-config --libs sdl2` -lSDL2_image -lm

SRC=main.c image_processing/image_manager.c
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: main

main: $(OBJ)

clean:
	${RM} ./**/*.o
	${RM} ./**/*.d
	${RM} main

# END