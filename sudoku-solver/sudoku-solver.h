#ifndef SUDOKU_SOLVER
#define SUDOKU_SOLVER

// N is define for the size of the 2D matrix
#define N 9

char *create(int grid[N][N]);
void print(int arr[N][N]);
int isSafe(int grid[N][N], int row,int col, int num);
int solveSuduko(int grid[N][N], int row, int col);

#endif