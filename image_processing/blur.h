#ifndef BLUR_H
#define BLUR_H

#include "../utils/image.h"

void gaussian_blur_image(Image *image, int range, double sigma, int K);

#endif