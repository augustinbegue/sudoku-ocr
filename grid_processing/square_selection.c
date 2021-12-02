#include <math.h>
#include "geometry.h"
#include "helpers.h"
#include "list.h"

static void swap_points(point *points, int i, int j)
{
    point tmp = points[i];
    points[i] = points[j];
    points[j] = tmp;
}

static int get_biggest_side(square *sq)
{
    int biggest;

    point c1 = sq->c1;
    point c2 = sq->c2;
    point c3 = sq->c3;
    point c4 = sq->c4;

    double side1_length
        = sqrt((c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
    double side2_length
        = sqrt((c3.x - c2.x) * (c3.x - c2.x) + (c3.y - c2.y) * (c3.y - c2.y));
    double side3_length
        = sqrt((c4.x - c3.x) * (c4.x - c3.x) + (c4.y - c3.y) * (c4.y - c3.y));
    double side4_length
        = sqrt((c4.x - c1.x) * (c4.x - c1.x) + (c4.y - c1.y) * (c4.y - c1.y));

    if (side1_length < side2_length)
    {
        biggest = side2_length;
    }
    else
    {
        biggest = side1_length;
    }
    if (side3_length > biggest)
    {
        biggest = side3_length;
    }
    if (side4_length > biggest)
    {
        biggest = side4_length;
    }

    return biggest;
}

/**
 * @brief Set the square's coordinates in the correct order.
 *
 * @param selected square to normalize
 */
void normalize_square(square *selected)
{
    point points[4] = {selected->c1, selected->c2, selected->c3, selected->c4};

    int delta = get_biggest_side(selected) / 10;

    int smallest_x = selected->c1.x;
    int smallest_y = selected->c1.y;
    int biggest_x = selected->c1.x;
    int biggest_y = selected->c1.y;

    for (int i = 0; i < 4; i++)
    {
        if (points[i].x < smallest_x)
        {
            smallest_x = points[i].x;
        }
        if (points[i].y < smallest_y)
        {
            smallest_y = points[i].y;
        }
        if (points[i].x > biggest_x)
        {
            biggest_x = points[i].x;
        }
        if (points[i].y > biggest_y)
        {
            biggest_y = points[i].y;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (points[i].x - smallest_x < delta
            && points[i].y - smallest_y < delta)
        {
            swap_points(points, i, 0);
        }
        if (biggest_x - points[i].x < delta
            && points[i].y - smallest_y < delta)
        {
            swap_points(points, i, 1);
        }
        if (biggest_x - points[i].x < delta && biggest_y - points[i].y < delta)
        {
            swap_points(points, i, 2);
        }
        if (points[i].x - smallest_x < delta
            && biggest_y - points[i].y < delta)
        {
            swap_points(points, i, 3);
        }
    }

    selected->c1 = points[0];
    selected->c2 = points[1];
    selected->c3 = points[2];
    selected->c4 = points[3];
}

square *select_square(
    list *squares, Image *image, bool verbose_mode, char *verbose_path)
{

    printf("   🔲 Selecting the grid square\n");

    square *selected = malloc(sizeof(square));

    list_node *sqr_el = squares->head;

    // Used to determine the best square to select by giving different weights
    // to some parameters
    double best_selection_factor = 0;

    while (sqr_el != NULL)
    {
        square *current = (square *)sqr_el->value;

        point c1 = current->c1;
        point c2 = current->c2;
        point c3 = current->c3;
        point c4 = current->c4;

        double side1_length = sqrt(
            (c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
        double side2_length = sqrt(
            (c3.x - c2.x) * (c3.x - c2.x) + (c3.y - c2.y) * (c3.y - c2.y));
        double side3_length = sqrt(
            (c4.x - c3.x) * (c4.x - c3.x) + (c4.y - c3.y) * (c4.y - c3.y));
        double side4_length = sqrt(
            (c4.x - c1.x) * (c4.x - c1.x) + (c4.y - c1.y) * (c4.y - c1.y));

        double smallest;
        double biggest;
        if (side1_length < side2_length)
        {
            smallest = side1_length;
            biggest = side2_length;
        }
        else
        {
            smallest = side2_length;
            biggest = side1_length;
        }

        if (side3_length < smallest)
        {
            smallest = side3_length;
        }
        else if (side3_length > biggest)
        {
            biggest = side3_length;
        }

        if (side4_length < smallest)
        {
            smallest = side4_length;
        }
        else if (side4_length > biggest)
        {
            biggest = side4_length;
        }

        double smallest_area = pow(smallest, 2);
        double biggest_area = pow(biggest, 2);

        double diff = biggest_area - smallest_area;
        // diff between biggest and smallest possible area / biggest area)
        double diff_factor = (diff / biggest_area) * smallest_area;

        double selection_factor = smallest_area * 3 - diff_factor * 10;

        if (selection_factor >= best_selection_factor)
        {
            *selected = *current;
            best_selection_factor = selection_factor;
        }

        sqr_el = sqr_el->next;
    }

    draw_square(image, selected, 255, 0, 0);
    square selected_2 = *selected;
    selected_2.c1.x--;
    selected_2.c1.y--;
    selected_2.c2.x--;
    selected_2.c2.y--;
    selected_2.c3.x--;
    selected_2.c3.y--;
    selected_2.c4.x--;
    selected_2.c4.y--;
    selected_2.c1.x--;
    selected_2.c1.y--;
    selected_2.c2.x--;
    selected_2.c2.y--;
    selected_2.c3.x--;
    selected_2.c3.y--;
    selected_2.c4.x--;
    selected_2.c4.y--;
    draw_square(image, &selected_2, 255, 0, 0);

    verbose_save(verbose_mode, verbose_path, "7.4-selected-square.png", image);

    return selected;
}