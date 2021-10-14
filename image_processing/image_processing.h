#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "../utils/image.h"

void process_image(
    Image *maskpt, Image *imagept, bool verbose, char *verbose_path);

#endif