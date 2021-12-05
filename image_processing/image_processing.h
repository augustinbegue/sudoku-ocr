#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "image.h"

void image_processing_extract_grid(
    Image *maskpt, Image *imagept, bool verbose, char *verbose_path);

void image_processing_extract_digits(
    Image *input, bool verbose_mode, char *verbose_path);

square image_processing_detect_digit_boundaries(Image *input);

#endif