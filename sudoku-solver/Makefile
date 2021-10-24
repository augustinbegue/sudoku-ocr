# Compilers vars
CC=gcc
CPPFLAGS= -MMD
CFLAGS= -Wall -Wextra -std=c99 -O3 -D__NO_INLINE__
LDFLAGS=
LDLIBS=

SRC=main.c solver.c
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: solver

solver: $(OBJ)

-include ${DEP}

clean:
	rm -f *.o
	rm -f ${DEP}
	rm -f ${PRG}
	rm -f solver
	rm -f grid_00.result

# END Makefile
