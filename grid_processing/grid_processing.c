#include <stdbool.h>
#include <stdio.h>
#include "automatic_rotation.h"
#include "edge_averaging.h"
#include "edge_detection.h"
#include "grid_splitting.h"
#include "hough_transform.h"
#include "image.h"
#include "int_list.h"
#include "list.h"
#include "square_detection.h"
#include "square_selection.h"

void grid_processing_split_image(
    Image *rotated_imagept, bool verbose_mode, char *verbose_path)
{
    Image edge_image
        = canny_edge_filtering(rotated_imagept, verbose_mode, verbose_path);

    int_list *edges_x = li_create();
    int_list *edges_y = li_create();

    Image lines_image = clone_image(rotated_imagept);
    Image *lines_imagept = &lines_image;

    int **hough_accumulator = hough_transform(&edge_image, lines_imagept,
        edges_x, edges_y, verbose_mode, verbose_path);

    int edge_num = 0;
    int **edges = average_edges(edges_x, edges_y, lines_imagept, verbose_mode,
        verbose_path, &edge_num);

    if (!verbose_mode)
        printf("   ⏹️ Finding squares...\n");

    list *squares = find_squares(edges, edge_num, lines_imagept);

    square *selected_square
        = select_square(squares, lines_imagept, verbose_mode, verbose_path);

    Image autorotated_image = automatic_rotation(
        hough_accumulator, selected_square, rotated_imagept, verbose_mode);

    verbose_save(
        verbose_mode, verbose_path, "8-autorotated.png", &autorotated_image);

    // Image **image_cells = split_grid(
    //     &autorotated_image, selected_square, verbose_mode, verbose_path);

    free_Image(&edge_image);
    free_Image(&autorotated_image);
    free_Image(lines_imagept);
    // for (int i = 0; i < 9; i++)
    // {
    //     for (int j = 0; j < 9; j++)
    //     {
    //         free_Image(image_cells[i * 9 + j]);
    //         free(image_cells[i * 9 + j]);
    //     }
    // }
    // free(image_cells);
    free_2d_arr(edges, edge_num);
    l_free_values(squares);
    li_free(edges_x);
    li_free(edges_y);
    free(selected_square);
}