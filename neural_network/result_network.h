#ifndef RESULT_NETWORK_H
#define RESULT_NETWORK_H

#include "maths_extra.h"
#include "matrix.h"

int result_network(
    Image *image, Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob);
int neural_network_execute(Image *image);

#endif