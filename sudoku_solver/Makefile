# Makefile

CC=gcc -I../image_processing -I../grid_processing -I../utils -I../image_transform -I../neural_network -I../sudoku_solver

# do not change
CPPFLAGS= `pkg-config --cflags gtk+-3.0` `pkg-config --cflags sdl2` -D__NO_INLINE__
CFLAGS= -Wall -Wextra  -std=c99 -O3 -g -fsanitize=address
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs gtk+-3.0` `pkg-config --libs sdl2` -lSDL2_image -lm -lpthread

SRC=solver.c $(wildcard ../image_processing/*.c) $(wildcard ../image_transform/*.c) $(wildcard ../utils/*.c) $(wildcard ../grid_processing/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: solver

solver: $(OBJ) main.o 

clean:
	${RM} $(OBJ) $(DEP) main

# END
