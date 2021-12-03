#include <stdio.h>
#include <stdlib.h>
#include "solver.h"

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

// Function to check if it is allowed to fill a number in the grid
int isSafe(int grid[N][N], int row, int col, int num)
{

    // Check if we find the same number in the similar row
    for (int x = 0; x <= 8; x++)
    {
        if (grid[row][x] == num)
        {
            return 0;
        }
    }

    // Check if we find the same number in the similar column
    for (int x = 0; x <= 8; x++)
    {
        if (grid[x][col] == num)
        {
            return 0;
        }
    }

    // Check if we find the same number in the 3*3 matrix
    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (grid[i + startRow][j + startCol] == num)
            {
                return 0;
            }
        }
    }

    return 1;
}

// Solver Function
int solveSuduko(int grid[N][N], int row, int col)
{
    if (row == N - 1 && col == N)
    {
        return 1;
    }

    if (col == N)
    {
        row++;
        col = 0;
    }

    if (grid[row][col] > 0)
    {
        return solveSuduko(grid, row, col + 1);
    }

    for (int num = 1; num <= N; num++)
    {
        if (isSafe(grid, row, col, num) == 1)
        {
            grid[row][col] = num;
            if (solveSuduko(grid, row, col + 1) == 1)
            {
                return 1;
            }
        }
        grid[row][col] = 0;
    }
    return 0;
}