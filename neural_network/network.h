#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "matrix.h"
#include "maths_extra.h"
#include "saveLoad.h"


int train();
double result_network(double i1, double i2, Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob);

#endif