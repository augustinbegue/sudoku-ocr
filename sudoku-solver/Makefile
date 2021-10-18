# Compilers vars
CC=gcc
CPPFLAGS= -MMD
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS=

SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: main

main: $(OBJ)

-include ${DEP}

clean:
	rm -f *.o
	rm -f ${DEP}
	rm -f ${PRG}

# END Makefile