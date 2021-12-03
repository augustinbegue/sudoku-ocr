#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "image.h"
#include "pixel_operations.h"

// N is the size of the 2D matrix   N*N
#define N 9

/* Function to read and store an array from a file */
void loadarray(char *filename, int grid[N][N])
{
    FILE *fp;
    fp = fopen(filename, "r");
    int i = 0;
    int j = 0;
    char charac;
    while ((charac = fgetc(fp)) != EOF)
    {
        if (charac != ' ')
        {
            if (charac == '\n')
            {
                j = 0;
                i++;
            }
            else if (charac == '.')
            {
                grid[i][j] = 0;
                j++;
            }
            else
            {
                grid[i][j] = charac - '0';
                j++;
            }
        }
    }
    fclose(fp);
}

/* Function to write a 2D array into a file*/
void writefile(int arr[N][N], char *filename)
{
    FILE *fp;
    fp = fopen(filename, "w");
    int i = 0;
    while (i < N)
    {
        for (int j = 0; j < N; j++)
        {
            if (j == 2 || j == 5)
            {
                fprintf(fp, "%d ", arr[i][j]);
            }
            else
            {
                fprintf(fp, "%d", arr[i][j]);
            }
        }
        fprintf(fp, "\n");
        if (i == 2 || i == 5)
        {
            fprintf(fp, "\n");
        }
        i++;
    }
    fclose(fp);
}

/* Function to print the matrix */
void print(int arr[N][N])
{
    int i = 0;
    while (i < N)
    {
        for (int j = 0; j < N; j++)
        {
            if (j == 2 || j == 5)
            {
                printf("%d ", arr[i][j]);
            }
            else
            {
                printf("%d", arr[i][j]);
            }
        }
        printf("\n");
        if (i == 2 || i == 5)
        {
            printf("\n");
        }
        i++;
    }
}

// Function to check if an entry is unassigned
int isUnassigned(int grid[N][N], int *row, int *col)
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            if (!grid[x][y])
            {
                *row = x;
                *col = y;
                return 1;
            }
        }
    }
    return 0;
}

// Function to check if the sudoku is correct
int isCorrect(int grid[N][N], int row, int col, int num)
{
    int brow = row / 3 * 3;
    int bcol = col / 3 * 3;

    for (int x = 0; x < 9; ++x)
    {
        if (grid[row][x] == num)
        {
            return 0;
        }
        if (grid[x][col] == num)
        {
            return 0;
        }
        if (grid[brow + (x % 3)][bcol + (x / 3)] == num)
        {
            return 0;
        }
    }
    return 1;
}

// Sudoku Solver
int solverSudoku(int grid[N][N])
{
    int row;
    int col;
    if (!isUnassigned(grid, &row, &col))
    {
        return 1;
    }

    for (int num = 1; num < 10; num++)
    {
        if (isCorrect(grid, row, col, num))
        {
            grid[row][col] = num;
            if (solverSudoku(grid))
            {
                return 1;
            }
            grid[row][col] = 0;
        }
    }
    return 0;
}

/* Function that returns a the solved grid */
int **solvedGrid(int grid[N][N])
{
    if (solverSudoku(grid))
    {
        return grid;
    }
    else
    {
        return NULL;
    }
}

/* Function to get the file which correspnd to the right input */
char *findPath(char *path, int i)
{
    int j = 0;
    while (path[i] != '\0')
    {
        if (path[j] == (char)i)
        {
            return 1;
        }
        i++;
    }
    return 0;
}

void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1, "Could not initialize SDL: %s.\n", SDL_GetError());
}

void replaceImage(Image *img, char *path, int width, int height)
{
    Image image = SDL_Surface_to_Image(load_image(path));
    for (int i = 0; i < width; i++)
    {
        for (in j = 0; j < height; j++)
        {
            Uint8 color = 0;

            Pixel pix = {color, color, color};

            image.pixels[i][j] = pix;
        }
    }
}

void Surfaceload(int grid[N][N], Image *image)
{
    int mwidth = image->w;
    int mheight = image->h;
    int width = 0;
    int height = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (width == mwidth && height == mheight)
            {
                width = 0;
                height = 0;
            }
            else
            {
                replaceImage(image, "../assets/grids/digit-%d.png", grid[i][j],
                    width / 9, height / 9);
                width += mheight / 9;
                height += mheight / 9;
            }
        }
    }
}