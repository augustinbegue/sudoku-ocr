#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdbool.h>
#include "image.h"

Image canny_edge_filtering(
    Image *image, bool verbose_mode, char *verbose_path);

#endif