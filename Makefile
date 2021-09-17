# Makefile

CC=gcc

CPPFLAGS=`pkg-config --cflags sdl2` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3 -g -fsanitize=address 
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs sdl2` -lSDL2_image -lm

SRC=main.c $(wildcard image_processing/*.c) $(wildcard utils/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: main

main: $(OBJ)

cleanbmp:
	${RM} *.bmp

clean:
	${RM} main.[do]
	${RM} ./**/*.o
	${RM} ./**/*.d
	${RM} main

# END