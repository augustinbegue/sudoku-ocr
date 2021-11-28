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

//Function to check if an entry is unassigned
int isUnassigned(int puzzle[][N], int *row, int *col) 
{
    for (int x = 0; x < 9; x++) 
    {
        for (int y = 0; y < 9; y++) 
        {
            if (!puzzle[x][y]) 
            {
                *row = x;
                *col = y;
                return 1;
            }
        }
    }
    return 0;
}

//Function to check if the sudoku is correct 
int isCorrect(int puzzle[][N], int row, int col, int num) 
{
    int brow = row / 3 * 3;
    int bcol = col / 3 * 3;

    for (int x = 0; x < 9; ++x) 
    {
        if (puzzle[row][x] == num)
        { 
            return 0;
        }
        if  (puzzle[x][col] == num) 
        {
            return 0;
        }
        if (puzzle[brow + (x % 3)][bcol + (x / 3)] == num) 
        {
            return 0;
        }
    }
    return 1;
}

//Sudoku Solver
int solverSudoku(int puzzle[][N]) 
{
    int row;
    int col;
    if(!isUnassigned(puzzle, &row, &col)) 
    {
        return 1;
    }
    
    for (int num = 1; num < 10; num++) 
    {
        if (isCorrect(puzzle, row, col, num)) 
        {
            puzzle[row][col] = num;
            if(solverSudoku(puzzle)) 
            {
                return 1;
            }  
            puzzle[row][col] = 0;
        }
    }
    return 0;
}

/* Function that returns a the solved grid */
int **solvedGrid(int grid[][N])
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