#include <math.h>
#include "geometry.h"
#include "helpers.h"
#include "list.h"

square *select_square(
    list *squares, Image *image, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
    {
        printf("   🔲 Selecting the grid square\n");
    }

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

        double ideal_area = pow(smallest, 2);
        double actual_area = pow(biggest, 2);

        double diff = actual_area - ideal_area;
        // diff between biggest and smallest possible area / biggest area)
        double diff_factor = (diff / actual_area) * ideal_area;

        // Parameters Weight for selection:
        // ideal_area * 1
        // diff_factor * 100
        double selection_factor = ideal_area - (diff_factor);

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