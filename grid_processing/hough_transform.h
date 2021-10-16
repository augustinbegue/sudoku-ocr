#ifndef HOUGH_H
#define HOUGH_H

#include <err.h>
#include <math.h>
#include "../utils/helpers.h"
#include "../utils/image.h"

Image hough_transform(
    Image *in, Image *clean, bool verbose_mode, char *verbose_path);

#endif