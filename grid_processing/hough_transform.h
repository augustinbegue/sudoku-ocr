#ifndef HOUGH_H
#define HOUGH_H

#include <err.h>
#include <math.h>
#include "helpers.h"
#include "image.h"
#include "list.h"

Image hough_transform(Image *in, Image *clean, list *edges_x, list *edges_y,
                      bool verbose_mode, char *verbose_path);

#endif