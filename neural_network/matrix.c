#include "matrix.h"
#include "maths_extra.h"

// all explanations of function are in the header

Matrix *m_init(Matrix *a, int rows, int cols)
{
    if (rows < 1 || cols < 1)
    {
        printf("m_init : Error on matrix size \n");
        exit(1);
    }

    a->rows = rows;
    a->cols = cols;
    a->data = malloc(sizeof(double) * rows * cols);

    for (int i = 0; i < rows * cols; i++)
    {
        (a->data)[i] = 0.0;
    }

    return a;
}

void m_free(Matrix *a)
{
    free(a->data);
}

Matrix *m_identity(Matrix *a, int size)
{
    if (size < 1)
    {
        printf("m_identity : Error on matrix size \n");
        exit(1);
    }

    a->rows = size;
    a->cols = size;
    a->data = malloc(sizeof(double) * size * size);

    return a;
}

/* Mutators */

Matrix *m_setIndex(Matrix *a, int r, int c, double val)
{
    a->data[r * (a->cols) + c] = val;
    return a;
}

Matrix *m_map_row(Matrix *a, int r, double (*f)(double))
{
    if (r >= a->rows || r < 0)
    {
        printf("Cannot map over row %d: index out of bounds", r);
        m_print(a);
        exit(1);
    }
    for (int j = 0; j < a->cols; j++)
    {
        double x = m_get(a, r, j);
        m_setIndex(a, r, j, f(x));
    }
    return a;
}
Matrix *m_map_col(Matrix *a, int c, double (*f)(double))
{
    if (c >= a->cols || c < 0)
    {
        printf("Cannot map over column %d: index out of bounds", c);
        m_print(a);
        exit(1);
    }
    for (int i = 0; i < a->rows; i++)
    {
        double x = m_get(a, i, c);
        m_setIndex(a, i, c, f(x));
    }
    return a;
}

Matrix *m_map(Matrix *a, double (*f)(double))
{
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            double x = m_get(a, i, j);
            m_setIndex(a, i, j, f(x));
        }
    }
    return a;
}

Matrix *m_add(Matrix *a, Matrix *b)
{
    if (a->rows == b->rows && a->cols == b->cols)
    {
        for (int i = 0; i < a->rows; i++)
        {
            for (int j = 0; j < b->cols; j++)
            {
                double x = m_get(a, i, j) + m_get(b, i, j);
                m_setIndex(a, i, j, x);
            }
        }
        return a;
    }
    else if (a->cols == b->cols && b->rows == 1)
    {
        for (int i = 0; i < a->rows; i++)
        {
            for (int j = 0; j < b->cols; j++)
            {
                double x = m_get(a, i, j) + m_get(b, 0, j);
                m_setIndex(a, i, j, x);
            }
        }
        return a;
    }
    else
    {
        printf("Cannot add matrices: not same dimensions.\n");
        m_full_print(a);
        m_full_print(b);
        exit(1);
    }
}

Matrix *m_subtract(Matrix *a, Matrix *b)
{
    if (a->rows == b->rows && a->cols == b->cols)
    {
        for (int i = 0; i < a->rows; i++)
        {
            for (int j = 0; j < b->cols; j++)
            {
                double x = m_get(a, i, j) - m_get(b, i, j);
                m_setIndex(a, i, j, x);
            }
        }
        return a;
    }
    else if (a->cols == b->cols && b->rows == 1)
    {
        for (int i = 0; i < a->rows; i++)
        {
            for (int j = 0; j < b->cols; j++)
            {
                double x = m_get(a, i, j) - m_get(b, 0, j);
                m_setIndex(a, i, j, x);
            }
        }
        return a;
    }
    else
    {
        printf("Cannot subtract matrices: not same dimensions.\n");      
        exit(1);
    }
}


Matrix *m_hadamard(Matrix *a, Matrix *b)
{
    if (a->rows != b->rows || a->cols != b->cols)
    {
        printf("Cannot Hadamard product matrices: Mismatched dimensions.\n");
       
     exit(1);
    }
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            double x = m_get(a, i, j) * m_get(b, i, j);
            m_setIndex(a, i, j, x);
        }
    }
    return a;
}

Matrix *m_scalar_mult(Matrix *a, double x)
{
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            double val = m_get(a, i, j) * x;
            m_setIndex(a, i, j, val);
        }
    }
    return a;
}

int max_mat(Matrix *a){
    double max = m_get(a, 0, 0);
    int max_ind = 0;
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            double val = m_get(a, i, j);
            if (val > max)
            {
                max = val;
                max_ind = j;
            }
        }
    }
    return max_ind;
}

Matrix *m_scalar_add(Matrix *a, double x)
{
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            double val = m_get(a, i, j) + x;
            m_setIndex(a, i, j, val);
        }
    }
    return a;
}

/* Accessors */

Matrix *m_mult(Matrix *a, Matrix *b, Matrix *dest)
{
    if (a->cols != b->rows)
    {
        printf("Cannot multiply matrices: Mismatched dimensions.\n");
        exit(1);
    }
    m_init(dest, a->rows, b->cols);
    for (int i = 0; i < dest->rows; i++)
    {
        for (int j = 0; j < dest->cols; j++)
        {
            double sum = 0;
            for (int k = 0; k < a->cols; k++)
            {
                sum += m_get(a, i, k) * m_get(b, k, j);
            }
            m_setIndex(dest, i, j, sum);
        }
    }
    return dest;
}

Matrix *m_transpose(Matrix *a, Matrix *dest)
{
    m_init(dest, a->cols, a->rows);
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            m_setIndex(dest, j, i, m_get(a, i, j));
        }
    }
    return dest;
}

Matrix *m_copy(Matrix *src, Matrix *dest)
{
    m_init(dest, src->rows, src->cols);
    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            m_setIndex(dest, i, j, m_get(src, i, j));
        }
    }
    return dest;
}

double m_get(Matrix *a, int r, int c)
{
    return a->data[r * (a->cols) + c];
}

int m_rows(Matrix *a)
{
    return a->rows;
}

int m_cols(Matrix *a)
{
    return a->cols;
}

double m_sum(Matrix *a)
{
    double total = 0.0;
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            total += m_get(a, i, j);
        }
    }
    return total;
}

Matrix *m_colsum(Matrix *a, Matrix *dest)
{
    m_init(dest, 1, a->cols);
    for (int j = 0; j < a->cols; j++)
    {
        double total = 0.0;
        for (int i = 0; i < a->rows; i++)
        {
            total += m_get(a, i, j);
        }
        m_setIndex(dest, 0, j, total);
    }
    return dest;
}

/* Ease of life */

void m_print(Matrix *a)
{
    printf("[");
    for (int i = 0; i < a->rows; i++)
    {
        if (i > 0)
        {
            printf(" [");
        }
        else
        {
            printf("[");
        }

        for (int j = 0; j < a->cols; j++)
        {
            printf(" %f ", m_get(a, i, j));
        }

        if (i == (a->rows) - 1)
        {
            printf("]");
        }
        else
        {
            printf("]\n");
        }
    }
    printf("]\n");
}

void m_full_print(Matrix *a)
{
    printf("Size: (%d, %d)\n", a->rows, a->cols);
    m_print(a);
}

bool m_equals(Matrix *a, Matrix *b)
{
    if (!(a->rows == b->rows) || !(a->cols == b->cols))
    {
        return false;
    }
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < a->cols; j++)
        {
            if (!(double_equals(m_get(a, i, j), m_get(b, i, j))))
            {
                return false;
            }
        }
    }
    return true;
}





Matrix *softmax(Matrix *src)
{
    double sum = 0;

    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            sum += exp(m_get(src, i, j));
        }
    }

    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            double tmp = exp(m_get(src, i, j))/sum;
            m_setIndex(src ,i, j, tmp);
        }
    }
    return src;
}

Matrix* d_softmax(Matrix* src)
{
    Matrix act;
    m_copy(src, &act);
  //double* buff = new double[size];
    softmax(&act);

    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            double val = m_get(&act, i, j);
            double tmp = val * (1. - val);
            m_setIndex(src ,i, j, tmp);
        }
    }
  /*for (int i = 0; i < size; i++) {
    buff[i] = act[i] * (1. - act[i]);
  }
  delete[] act;
  return buff;
  */
    m_free(&act);
    return src;
}