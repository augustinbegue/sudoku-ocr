#include <stdio.h>
#include <stdlib.h>
#include "sudoku-solver.h"
 
// N is the size of the 2D matrix   N*N
#define N 9

/* Function to create an array of char fro ma matrix of int */
char *create(int grid[N][N])
{
    int pos = 0;
    char *arr = malloc(sizeof(int) * 81);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++, pos++)
        {
            arr[pos] = grid[i][j] + '0';
        }
    }
    return arr;
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
int isSafe(int grid[N][N], int row,int col, int num)
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
 
    // Check if we find the same number in the particular 3*3 matrix
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