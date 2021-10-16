#ifndef LINES_AVG_H
#define LINES_AVG_H

#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "list.h"

int **average__hough_lines(list *edges_x, list *edges_y, Image *in,
    bool verbose_mode, char *verbose_path, int *new_edge_num_);

#endif