#ifndef LINES_AVG_H
#define LINES_AVG_H

#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "list.h"

int **average_edges(list *edges_x, list *edges_y, Image *image,
    bool verbose_mode, char *verbose_path, int *new_edge_num_);

#endif