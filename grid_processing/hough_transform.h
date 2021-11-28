#ifndef HOUGH_H
#define HOUGH_H

#include <err.h>
#include <math.h>
#include "helpers.h"
#include "image.h"
#include "int_list.h"

int **hough_transform(Image *in, Image *clean, int_list *edges_x,
    int_list *edges_y, bool verbose_mode, char *verbose_path);

#endif