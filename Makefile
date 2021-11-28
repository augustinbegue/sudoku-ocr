# Makefile

CC=gcc -I./image_processing -I./grid_processing -I./utils -I./image_transform -I./neural_network

# do not change
CPPFLAGS= `pkg-config --cflags gtk+-3.0` `pkg-config --cflags sdl2` -D__NO_INLINE__
CFLAGS= -Wall -Wextra  -std=c99 -O3 -g -fsanitize=address
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs gtk+-3.0` `pkg-config --libs sdl2` -lSDL2_image -lm -lpthread

SRC=$(wildcard image_processing/*.c) $(wildcard image_transform/*.c) $(wildcard utils/*.c) $(wildcard grid_processing/*.c)  $(wildcard neural_network/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: sudoku-ocr-gui sudoku-ocr network solver

sudoku-ocr-gui: sudoku-ocr-gui.o $(OBJ)

network: network.o $(OBJ)

solver:
	make -C ./sudoku-solver

sudoku-ocr: sudoku-ocr.o $(OBJ)

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
