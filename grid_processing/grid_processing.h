#ifndef GRID_PROCESSING_H
#define GRID_PROCESSING_H

#include <stdbool.h>
#include "image.h"

square *grid_processing_detect_grid(Image *rotated_imagept,
    double *rotation_amount, bool verbose_mode, char *verbose_path);

#endif