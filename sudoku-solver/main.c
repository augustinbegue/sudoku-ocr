#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sudoku-solver.h"

int main()
{
    int grid[N][N];
    loadarray("grid_00", grid);
    
    if (solveSuduko(grid, 0, 0) == 1)
    {
        writefile(grid, "grid_00.result");
    }
    else
    {
        printf("No solution exists");
    }
 
    return 0;
}