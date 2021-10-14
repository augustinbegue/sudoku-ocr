#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include <stdbool.h>
#include "../utils/image.h"

void find_edges_image(Image *image, bool verbose_mode, char *verbose_path);

#endif