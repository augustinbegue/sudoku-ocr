#include <float.h>
#include <stdbool.h>
#include "geometry.h"

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