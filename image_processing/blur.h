#ifndef BLUR_H
#define BLUR_H

#include "image.h"

void gaussian_blur_image(Image *image, int range, double sigma, int K);

double *get_gaussian_smoothing_kernel(int range, double sigma);

#endif