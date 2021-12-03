#ifndef SOLVER
#define SOLVER

// N is define for the size of the 2D matrix
#define N 9

void loadarray(char *filename, int grid[N][N]);
void writefile(int arr[N][N], char *filename);
void print(int arr[N][N]);
int find_empty_cell(int puzzle[][N], int *row, int *column);
int valid(int puzzle[][N], int row, int column, int guess);
int solverSudoku(int puzzle[][N]);
int **solvedGrid(int grid[][N]);
// Image *Display_solved_sudoku(int grid[N][N]);

#endif