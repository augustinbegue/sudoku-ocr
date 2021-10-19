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
    double area_max = 0;

    while (sqr_el->next != NULL)
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
        if (side1_length < side2_length)
            smallest = side1_length;
        else
            smallest = side2_length;

        if (side3_length < smallest)
            smallest = side3_length;

        if (side4_length < smallest)
            smallest = side4_length;

        double area = pow(smallest, 2);

        if (area > area_max)
        {
            *selected = *current;
            area_max = area;
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