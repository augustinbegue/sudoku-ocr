#include <stdio.h>
#include <stdlib.h>

/* returns address of allocated array, returns NULL on failure */
int **create(int rows, int cols)
{
    int row = 0;
    int tmp = 0;
    int **arr = malloc(sizeof(int) * 81);

    if(arr)
    {
        for(row = 0; row < rows; ++row)
        {
            if(!(arr[row] = calloc(cols, sizeof **arr)))
            {
                for(tmp = 0; tmp < row; ++tmp)
                {
                    free(arr[tmp]);
                }

                free(arr);
                arr = 0;
                break;
            }
        }
    }
}

/* returns 0 for success, nonzero for failure */
int write(FILE *fp, int **arr, int rows, int cols)
{
    int row = 0;
    int col = 0;

    for(row = 0; row < rows; ++row)
    {
        if(fwrite(arr[row], sizeof **arr, cols, fp) < cols)
        {
            break;
        }
    }
    return row < rows;
}

/* store test data in array */
void store(int **arr, int rows, int cols)
{
    int row = 0;
    int col = 0;

    for(row = 0; row < rows; ++row)
    {
        for(col = 0; col < cols; ++col)
        {
            arr[row][col] = row * cols + col;
        }
    }
}

/* display array element values */
void show(int **arr, int rows, int cols)
{
    int row = 0;
    int col = 0;

    for(row = 0; row < rows; ++row)
    {
        for(col = 0; col < cols; ++col)
        {
            printf("%3d", arr[row][col]);
        }

        putchar('\n');
    }
}

int main()
{
    const int rows = 5;
    const int cols = 10;

    FILE *f = 0;
    int **array = create(rows, cols);

    if(!array)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        return EXIT_FAILURE;
    }

    store(array, rows, cols);
    show(array, rows, cols);

    f = fopen("data", "wb");

    if(!f)
    {
        fprintf(stderr, "Cannot open file for writing\n");
        free(array);
        return EXIT_FAILURE;
    }

    if(write(f, array, rows, cols))
    {
        fprintf(stderr, "Error writing to file\n");
    }

    if(fclose(f))
    {
        fprintf(stderr, "Error closing file\n");
    }

    free(array);
    return 0;
}