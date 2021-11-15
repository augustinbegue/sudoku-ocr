#ifndef GRID_PROCESSING_H
#define GRID_PROCESSING_H

#include "image.h"
#include <stdbool.h>

square *grid_processing_detect_grid(
    Image *rotated_imagept, bool verbose_mode, char *verbose_path);

#endif