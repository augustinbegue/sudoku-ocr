#ifndef ROTATION_ANGLE_H

#define ROTATION_ANGLE_H

#include "geometry.h"
#include "image.h"

Image automatic_rotation(int **hough_accumulator, square *found_grid_square,
    Image *in, double *rotation_amount, bool verbose_mode);

#endif