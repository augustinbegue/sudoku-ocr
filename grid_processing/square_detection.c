#include <err.h>
#include <float.h>
#include <stdbool.h>
#include "geometry.h"
#include "image.h"
#include "int_list.h"
#include "list.h"

line edge_to_line(int *edge)
{
    line edge1;

    edge1.x0 = edge[0];
    edge1.y0 = edge[1];
    edge1.x1 = edge[2];
    edge1.y1 = edge[3];

    return edge1;
}

bool lines_equal(line l1, line l2)
{
    return l1.x0 == l2.x0 && l1.x1 == l2.x1 && l1.y0 == l2.y0
           && l1.y1 == l2.y1;
}

bool points_equal(point p1, point p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

point line_intersect(line line1, line line2)
{
    point res;

    // Line 1 represented as a1 + b1 = c1
    double a1 = line1.y1 - line1.y0;
    double b1 = line1.x0 - line1.x1;
    double c1 = a1 * line1.x0 + b1 * line1.y0;

    // Line 2 represented as a2 + b2 = c2
    double a2 = line2.y1 - line2.y0;
    double b2 = line2.x0 - line2.x1;
    double c2 = a2 * line2.x0 + b2 * line2.y0;

    double determinant = a1 * b2 - a2 * b1;

    if (determinant == 0)
    {
        // Lines are parallel.
        res.x = FLT_MIN;
        res.y = FLT_MIN;

        return res;
    }
    else
    {
        res.x = (b2 * c1 - b1 * c2) / determinant;
        res.y = (a1 * c2 - a2 * c1) / determinant;

        return res;
    }
}

intersection *find_line_intersections(
    int **edges, line edge1, int edge_num, int w, int h, int *intersection_num)
{

    int_list *intersections_x = li_create();
    int_list *intersections_y = li_create();
    int_list *edges_index = li_create();

    // iterate over the other edges
    for (int j = 0; j < edge_num; j++)
    {
        line edge2 = edge_to_line(edges[j]);

        // continue if the lines are equal
        if (lines_equal(edge1, edge2))
        {
            continue;
        }

        point intersection_pt = line_intersect(edge1, edge2);

        if (intersection_pt.x < 0 || intersection_pt.x >= w
            || intersection_pt.x >= w || intersection_pt.y < 0
            || intersection_pt.y >= h || intersection_pt.y >= h)
        {
            // if the points are not in the boundaries of the image, we
            // consider they do not intersect
            continue;
        }
        else
        {
            // lines intersect
            li_append(intersections_x, intersection_pt.x);
            li_append(intersections_y, intersection_pt.y);
            li_append(edges_index, j);
        }
    }

    // Convert the lists to an array
    int_list_node *edge_index_el = edges_index->head;
    int_list_node *inter_x_el = intersections_x->head;
    int_list_node *inter_y_el = intersections_y->head;

    *intersection_num = li_size(edges_index);
    intersection *intersections
        = malloc(sizeof(intersection) * (*intersection_num) + 1);

    if (intersections == NULL)
    {
        errx(1, "error when allocating memory in find_line_intersections");
    }

    for (int i = 0; i < (*intersection_num); i++)
    {
        point ppt;
        ppt.x = inter_x_el->value;
        ppt.y = inter_y_el->value;

        intersection inter;
        inter.line1 = edge1;
        inter.line2 = edge_to_line(edges[edge_index_el->value]);
        inter.pt = ppt;

        intersections[i] = inter;

        edge_index_el = edge_index_el->next;
        inter_x_el = inter_x_el->next;
        inter_y_el = inter_y_el->next;
    }

    li_free(intersections_x);
    li_free(intersections_y);
    li_free(edges_index);

    return intersections;
}

list *find_line_squares(int **edges, line edge_1, int edge_num, Image *image)
{
    list *squares = l_create();

    int intersection_num_1 = 0;

    point zero = {.x = 0, .y = 0};

    intersection *intersections_1 = find_line_intersections(edges, edge_1,
        edge_num, image->width, image->height, &intersection_num_1);

    for (int j = 0; j < intersection_num_1; j++)
    {
        int intersection_num_2 = 0;
        line edge_2 = intersections_1[j].line2;
        point c1 = intersections_1[j].pt;

        if (lines_equal(edge_2, edge_1))
            continue;

        if (points_equal(c1, zero))
            continue;

        intersection *intersections_2 = find_line_intersections(edges, edge_2,
            edge_num, image->width, image->height, &intersection_num_2);

        for (int k = 0; k < intersection_num_2; k++)
        {
            int intersection_num_3 = 0;
            line edge_3 = intersections_2[k].line2;
            point c2 = intersections_2[k].pt;

            if (lines_equal(edge_3, edge_1) || lines_equal(edge_3, edge_2))
                continue;

            if (points_equal(c2, zero) || points_equal(c1, c2))
                continue;

            intersection *intersections_3
                = find_line_intersections(edges, edge_3, edge_num,
                    image->width, image->height, &intersection_num_3);

            for (int l = 0; l < intersection_num_3; l++)
            {
                int intersection_num_4 = 0;
                line edge_4 = intersections_3[l].line2;
                point c3 = intersections_3[l].pt;

                if (lines_equal(edge_4, edge_1) || lines_equal(edge_4, edge_2)
                    || lines_equal(edge_4, edge_3))
                    continue;

                if (points_equal(c3, zero) || points_equal(c3, c1)
                    || points_equal(c3, c2))
                    continue;

                intersection *intersections_4
                    = find_line_intersections(edges, edge_4, edge_num,
                        image->width, image->height, &intersection_num_4);

                for (int m = 0; m < intersection_num_4; m++)
                {
                    line edge_5 = intersections_4[m].line2;
                    point c4 = intersections_4[m].pt;

                    if (points_equal(c4, zero) || points_equal(c4, c1)
                        || points_equal(c4, c2) || points_equal(c4, c3))
                        continue;

                    if (lines_equal(edge_5, edge_1))
                    {
                        square *sqr = malloc(sizeof(square));
                        sqr->c1 = c1;
                        sqr->c2 = c2;
                        sqr->c3 = c3;
                        sqr->c4 = c4;

                        l_append(squares, sqr);

                        draw_square(image, sqr, 0, 255, 0);
                    }
                }

                free(intersections_4);
            }

            free(intersections_3);
        }

        free(intersections_2);
    }

    free(intersections_1);

    return squares;
}

list *find_squares(int **edges, int edge_num, Image *image)
{
    int i = 0;
    fprintf(stderr, "\33[2K\r   🖨️ Treated Edges: %i", i);
    list *found_squares = l_create();

    for (; i < edge_num; i++)
    {
        list *found = find_line_squares(
            edges, edge_to_line(edges[i]), edge_num, image);
        l_merge(found_squares, found);

        // only free the container so the merged nodes are not lost
        free(found);

        fprintf(stderr, "\33[2K\r   🖨️ Treated Edges: %i", i);
    }
    fprintf(stderr, "\33[2K\r   🖨️ Treated Edges: %i\n", i);

    return found_squares;
}