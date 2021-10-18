#ifndef MORE_MATH_H
#define MORE_MATH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// absolute value of a double
double abs_val(double x);

bool double_equals(double a, double b);

// the sigmoid function (compute the output of a neurone)
double sigmoid(double x);

// its derivate
double d_sigmoid(double x);

//
double rand_weight();

#endif