#include <math.h>
#include "geometry.h"
#include "helpers.h"
#include "list.h"

square *select_square(
    list *squares, Image *image, bool verbose_mode, char *verbose_path)
{
    square *selected = malloc(sizeof(square));

    list_node *sqr = squares->head;
    double area_max = 0;

    while (sqr != NULL)
    {
        sqr = sqr->next;

        square *current = (square *)sqr->value;

        point c1 = current->c1;
        point c2 = current->c2;

        double side_length = sqrt(
            (c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
        double area = pow(side_length, 2);

        if (area > area_max)
        {
            selected = sqr->value;
            area = area_max;
        }
    }

    draw_square(image, selected, 255, 255, 0);

    return selected;
}