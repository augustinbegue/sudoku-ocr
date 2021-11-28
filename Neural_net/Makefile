# Source, Executable, Includes, Library Defines
INCL   = $(wildcard *.h)
SRC    = $(wildcard *.c)
OBJ    = $(SRC:.c=.o)
LIBS   = -lm
EXE    = network

# Compiler, Linker Defines
CC=gcc -I../image_processing -I../grid_processing -I../utils -I../image_transform
CFLAGS  = -std=c99 -Wall -O2 -D__NO_INLINE__
LIBPATH = -L.
LDFLAGS = -o $(EXE) $(LIBPATH) $(LIBS)
CFDEBUG = -std=c99c -Wall -g -DDEBUG $(LDFLAGS)
RM      = rm -f

# Compile and Assemble C Source Files into Object Files
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c

# Link all Object Files with external Libraries into Binaries
$(EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS)

# Objects depend on these Libraries
$(OBJ): $(INCL)

# Create a gdb/dbx Capable Executable with DEBUG flags turned on
debug:
	$(CC) $(CFDEBUG) $(SRC)

# Clean Up Objects, Exectuables, Dumps out of source directory
clean:
	$(RM) $(OBJ) $(EXE) core a.out 

run:
	@./$(EXE) || true
