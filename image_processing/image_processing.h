#ifndef IMG_PROCESSING_H
#define IMG_PROCESSING_H

#include "../utils/image.h"

void process_image(Image *maskpt, Image *imagept, bool save_mask,
                   char *mask_output_path);

#endif