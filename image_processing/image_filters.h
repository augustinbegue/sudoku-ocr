#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <err.h>
#include <stdio.h>
#include "../utils/image.h"

void filter_grayscale(Image *image, double brightness);

void filter_bw(Image *image, double threshold);

void filter_contrast(Image *image, double value);

#endif