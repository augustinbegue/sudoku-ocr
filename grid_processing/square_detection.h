#ifndef SQUARE_DETECT_H
#define SQUARE_DETECT_H

#include "geometry.h"
#include "image.h"
#include "list.h"

list *find_squares(int **edges, int edge_num, Image *image, bool gtk);

#endif