#ifndef SQUARE_SELECT_H
#define SQUARE_SELECT_H

#include "geometry.h"

square *select_square(list *squares, Image *image, bool verbose_mode, char *verbose_path);
void normalize_square(square *selected);

#endif