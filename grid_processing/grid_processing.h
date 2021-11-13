#ifndef GRID_PROCESSING_H
#define GRID_PROCESSING_H

#include "image.h"
#include <stdbool.h>

void grid_processing_split_image(
    Image *rotated_imagept, bool verbose_mode, char *verbose_path);

#endif