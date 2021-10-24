#ifndef SOLVER
#define SOLVER

// N is define for the size of the 2D matrix
#define N 9

void loadarray(char *filename, int grid[N][N]);
void writefile(int arr[N][N], char *filename);
void print(int arr[N][N]);
int isSafe(int grid[N][N], int row,int col, int num);
int solveSuduko(int grid[N][N], int row, int col);

#endif