#include "maths_extra.h"

double abs_val(double x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}

static const double EPSILON = 0.00000001;

bool double_equals(double a, double b)
{
    return abs_val(a - b) < EPSILON;
}

double sigmoid(double x)
{
    return (1.0 / (1.0 + exp(-x)));
}

double d_sigmoid(double x)
{
    return (x * (1.0 - x));
}

double rand_weight()
{
    return ((double)rand()) / ((double)RAND_MAX);
}