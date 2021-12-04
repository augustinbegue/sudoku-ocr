#ifndef SOLVER
#define SOLVER

#include "../utils/image.h"

// N is define for the size of the 2D matrix
#define N 9

void loadarray(char *filename, int grid[N][N]);
void writefile(int arr[N][N], char *filename);
void print(int arr[N][N]);
int find_empty_cell(int puzzle[][N], int *row, int *column);
int valid(int puzzle[][N], int row, int column, int guess);
int solverSudoku(int puzzle[][N]);
int **solvedGrid(int grid[][N]);
void init_sdl();
void replaceImage(Image *img, char *path, int width, int height, Pixel pix);
Pixel findPixel(int grid[N][N], int solvedgrid[N][N]);
void displayEmptyGrid(int grid[N][N], int solvedGrid[N][N], Image *img);

#endif
