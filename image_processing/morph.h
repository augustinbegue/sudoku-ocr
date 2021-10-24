#ifndef MORPH_H
#define MORPH_H

#include "image.h"

enum MorphType
{
    Dilation = 0,
    Erosion = 1
};
typedef enum MorphType MorphType;

void morph(Image *image, MorphType morph_type, int mask_size);

#endif