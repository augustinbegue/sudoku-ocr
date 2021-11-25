#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "image.h"

void image_processing_extract_grid(
    Image *maskpt, Image *imagept, bool verbose, char *verbose_path, bool gtk);

#endif