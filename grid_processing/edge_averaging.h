#ifndef LINES_AVG_H
#define LINES_AVG_H

#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "int_list.h"

int **average_edges(int_list *edges_x, int_list *edges_y, Image *image,
    bool verbose_mode, char *verbose_path, int *new_edge_num_);

#endif