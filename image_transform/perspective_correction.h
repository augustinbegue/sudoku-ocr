#ifndef PERSPECTIVE_CORRECTION_H
#define PERSPECTIVE_CORRECTION_H

#include <stdbool.h>
#include "image.h"

Image correct_perspective(Image *rotated_imagept, square *selected_square,
                          bool verbose_mode, char *verbose_path);

#endif