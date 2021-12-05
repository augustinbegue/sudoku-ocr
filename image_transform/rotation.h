#ifndef ROTATION_H

#define ROTATION_H

#include "image.h"

void corrected_rotation_matrix(double angle, double x, double y,
                               double center_x, double center_y, double *rx, double *ry);

Image rotate_image(Image *image, double angle);

#endif