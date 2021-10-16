# Makefile

CC=gcc -I./image_processing -I./grid_processing -I./utils -I./image_rotation 

# do not change
CPPFLAGS=`pkg-config --cflags sdl2` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3 -g -fsanitize=address 
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs sdl2` -lSDL2_image -lm

SRC=main.c $(wildcard image_processing/*.c) $(wildcard image_rotation/*.c) $(wildcard utils/*.c) $(wildcard grid_processing/*.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: main

main: $(OBJ)

# unecessary for other projects
check:
	./testall.sh

# unecessary for other projects
clean-output:
	${RM} *.bmp
	${RM} *.csv
	${RM} ./**/*.bmp
	${RM} ./**/*.csv
	${RM} ./assets/output/*.png

clean: clean-output
	${RM} $(OBJ) $(DEP) main

# END