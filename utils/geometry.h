#ifndef GEOMETRY_H

#define GEOMETRY_H

#include <stdbool.h>

struct line
{
    int x0;
    int x1;
    int y0;
    int y1;
};
typedef struct line line;

struct point
{
    int x;
    int y;
};
typedef struct point point;

struct intersection
{
    line line1;
    line line2;
    point pt;
};
typedef struct intersection intersection;

struct square
{
    point c1;
    point c2;
    point c3;
    point c4;
};
typedef struct square square;

line edge_to_line(int *edge);
bool lines_equal(line l1, line l2);
bool points_equal(point p1, point p2);
point line_intersect(line line1, line line2);

#endif

