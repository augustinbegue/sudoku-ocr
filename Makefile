# Makefile

CC=gcc -I./image_processing -I./grid_processing -I./utils -I./image_rotation 

# do not change
CPPFLAGS=`pkg-config --cflags sdl2` -D__NO_INLINE__
CFLAGS= -Wall -Wextra  -std=c99 -O3 -g -fsanitize=address 
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs sdl2` -lSDL2_image -lm

SRC=main.c $(wildcard image_processing/*.c) $(wildcard image_rotation/*.c) $(wildcard utils/*.c) $(wildcard grid_processing/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: main network solver

main: $(OBJ)

network:
	make -C ./XOR

solver:
	make -C ./sudoku-solver

# unecessary for other projects
check:
	./testall.sh

# unecessary for other projects
clean-output:
	${RM} *.bmp
	${RM} *.csv
	${RM} ./**/*.bmp
	${RM} ./**/*.csv
	${RM} -r ./assets/output/*

clean: clean-output
	${RM} $(OBJ) $(DEP) main
	make clean -C ./sudoku-solver
	make clean -C ./XOR

# END
