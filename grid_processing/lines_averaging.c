#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "list.h"
// Amount we divide the biggest side of the image by to get the error factor
static const int ERR_FACTOR = 50;

/**
 * @brief Reduces the number of lines contained in edges_[xy] by averaging the
 * similar ones
 *
 * @warning edges_x and edges_y MUST have the same size.
 *
 * @param edges_x
 * @param edges_y
 * @param in
 * @return int**
 */
int **average__hough_lines(list *edges_x, list *edges_y, Image *in,
    bool verbose_mode, char *verbose_path, int *new_edge_num_)
{
    if (verbose_mode)
        printf("   🗺️ Averaging lines...\n");

    int w = in->width, h = in->height;
    int err_factor = (w > h ? w : h) / ERR_FACTOR;
    int size = l_size(edges_x);

    int **edges = coord_lists_to_arr(edges_x, edges_y, size);

    list *avg_edges_x = l_create();
    list *avg_edges_y = l_create();
    list_int *x0_el;
    list_int *y0_el;
    list_int *x1_el;
    list_int *y1_el;

    for (int i = 0; i < size / 2; i++)
    {
        int *edge = edges[i];
        int x0 = edge[0];
        int y0 = edge[1];
        int x1 = edge[2];
        int y1 = edge[3];

        bool found = false;
        x0_el = avg_edges_x->head;
        y0_el = avg_edges_y->head;

        if (x0_el != NULL)
        {
            x1_el = x0_el->next;
            y1_el = y0_el->next;

            while (y1_el != NULL && !found)
            {
                int ax0 = x0_el->value;
                int ay0 = y0_el->value;

                int ax1 = x1_el->value;
                int ay1 = y1_el->value;
                if ((abs(ax0 - x0) <= err_factor
                        && abs(ay0 - y0) <= err_factor)
                    || (abs(ax1 - x1) <= err_factor
                        && abs(ay1 - y1) <= err_factor))
                {
                    x0_el->value = (ax0 + x0) / 2;
                    y0_el->value = (ay0 + y0) / 2;

                    x1_el->value = (ax1 + x1) / 2;
                    y1_el->value = (ay1 + y1) / 2;

                    found = true;
                }
                else
                {
                    // Increment two times to get the next coordinates set
                    x0_el = x0_el->next; // x0 = x1
                    y0_el = y0_el->next; // y0 = y1
                    x1_el = x1_el->next; // x1 = new x0
                    y1_el = y1_el->next; // y1 = new y0

                    if (x1_el != NULL)
                    {
                        x0_el = x0_el->next; // x0 = new x0
                        y0_el = y0_el->next; // y0 = new y1
                        x1_el = x1_el->next; // x1 = new x1
                        y1_el = y1_el->next; // y1 = new y1
                    }
                    else
                    {
                        // We reached the end of the lists, we couldnt get full
                        // coordinates
                        break;
                    }
                }
            }
        }

        if (!found)
        {
            l_append(avg_edges_x, x0);
            l_append(avg_edges_y, y0);
            l_append(avg_edges_x, x1);
            l_append(avg_edges_y, y1);
        }
    }

    int new_size = l_size(avg_edges_x);
    int **new_edges = coord_lists_to_arr(avg_edges_x, avg_edges_y, new_size);

    l_free(avg_edges_x);
    l_free(avg_edges_y);

    for (int i = 0; i < new_size / 2; i++)
    {
        int *edge = new_edges[i];
        free(draw_line(in, w, h, edge[0], edge[1] + 1, edge[2], edge[3] + 1,
            255, 0, 255));
        free(draw_line(in, w, h, edge[0] + 1, edge[1], edge[2] + 1, edge[3],
            255, 0, 255));
    }

    free_2d_arr(edges, size / 2);

    if (verbose_mode)
        printf("   🤏 Reduced the number of lines from %i to %i\n", size,
            new_size);

    verbose_save(verbose_mode, verbose_path, "7.3-average-lines.png", in);

    *new_edge_num_ = new_size / 2;
    return new_edges;
}