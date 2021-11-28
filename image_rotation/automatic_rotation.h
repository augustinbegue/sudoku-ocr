#ifndef ROTATION_ANGLE_H

#define ROTATION_ANGLE_H

#include "geometry.h"
#include "image.h"

Image automatic_rotation(int **hough_accumulator, square *found_grid_square,
    Image *in, bool verbose_mode, char *verbose_path);

#endif