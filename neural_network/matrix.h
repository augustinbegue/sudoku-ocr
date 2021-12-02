#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define MATRIX_INITIAL_CAPACITY 4

typedef struct
{
    int rows;
    int cols;
    double *data;
} Matrix;



// initilize a matrix
Matrix *m_init(Matrix *a, int rows, int cols);

// indentity matrix of size: size
Matrix *m_identity(Matrix *a, int size);

// free the matrix from the memory 
void m_free(Matrix *a);

// set the value at a given index
Matrix *m_setIndex(Matrix *a, int r, int c, double val);

// map all value of a row according to the function given in parameter 
Matrix *m_map_row(Matrix *a, int r, double (*f)(double));

// map all value of a column according to the function given in parameter 
Matrix *m_map_col(Matrix *a, int c, double (*f)(double));

// map all value of the matrix according to the function given in parameter 
Matrix *m_map(Matrix *a, double (*f)(double));

// add matrix b to a
Matrix *m_add(Matrix *a, Matrix *b);

// substract matrix b to a 
Matrix *m_subtract(Matrix *a, Matrix *b);

// the matrix resulting is the matrix where indexes of same place are multiplied (it's just named hadamard product...)
Matrix *m_hadamard(Matrix *a, Matrix *b);

// multiply a matrix by a single double
Matrix *m_scalar_mult(Matrix *a, double x);

// add a double to each value of the matrix
Matrix *m_scalar_add(Matrix *a, double x);

int max_mat(Matrix *a);

// return a new matrix being the product of a and b 
Matrix *m_mult(Matrix *a, Matrix *b, Matrix *dest);

// return a new matrix being the transpose 
Matrix *m_transpose(Matrix *a, Matrix *dest);

// return a new matrix being the copy of the given in parameter
Matrix *m_copy(Matrix *src, Matrix *dest);


double m_get(Matrix *a, int r, int c);
int m_rows(Matrix *a);
int m_cols(Matrix *a);

// return the sum of all values
double m_sum(Matrix *a);

// return a matrix with the sum of values column by column
Matrix *m_colsum(Matrix *a, Matrix *dest);

// basic things 

void m_print(Matrix *a);
void m_full_print(Matrix *a);
bool m_equals(Matrix *a, Matrix *b);

Matrix *softmax(Matrix *src);
Matrix* d_softmax(Matrix* src);

int is_nan(Matrix *src);

double max_mat_value(Matrix *a);

#endif