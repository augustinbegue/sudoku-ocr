#ifndef FILTERS_H
#define FILTERS_H

#include <err.h>
#include <stdio.h>
#include "../utils/image.h"

void filter_grayscale(Image *image, double brightness);

void filter_bw(Image *image, double threshold);

void filter_contrast(Image *image, double value);

void filter_dynamic_threshold(Image *image, int subdivisions);

void filter_threshold(Image *image);

void filter_gamma(Image *image, double value);

void filter_invert(Image *image, double value);

#endif