# Makefile

CC=gcc

# do not change
CPPFLAGS=`pkg-config --cflags sdl2` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3 -g -fsanitize=address 
LDFLAGS= -fsanitize=address 
LDLIBS= `pkg-config --libs sdl2` -lSDL2_image -lm

# Files to use during the compilation
SRC=main.c $(wildcard image_processing/*.c) $(wildcard utils/*.c)

# do not change
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

clean:
	${RM} $(OBJ) $(DEP) main

# END