#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "image.h"
#include "solver.h"

int main(int argc, char **argv)
{
    int grid[N][N];
    loadarray(argv[1], grid);
    print(grid);

    if (argc != 2)
    {
        errx(1, "Wrong number of arguments.");
    }
    else if (argc == 0)
    {
        errx(1, "Not enough arguments entered.");
    }
    else if (solverSudoku(grid) == 1)
    {
        char *c = ".result";
        char *extension = argv[1];
        strncat(extension, c, 7);
        writefile(grid, extension);
        print(solvedGrid(grid));
        // Image *Display_solved_sudoku(grid);
    }
    else
    {
        printf("No solution exists\n");
    }

    return 0;
}