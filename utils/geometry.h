#ifndef GEOMETRY_H

#define GEOMETRY_H

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

#endif

