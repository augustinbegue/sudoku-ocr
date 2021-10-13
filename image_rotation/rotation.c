#include <math.h>
#include "../utils/image.h"

/**
 * @brief Rotates image by angle
 *
 * @param image image to rotate
 * @param angle angle of the rotation
 */
void rotate_image(Image *image, double angle)
{
    printf("...🔃 Rotating image by %.2f°\n", angle);

    image->height = image->height;
}

/**
 * @brief perform a rotation on the coordinates (x, y) by angle
 *
 * @param angle
 * @param x
 * @param y
 * @param rx rotated x coordinate
 * @param ry rotated y coordinate
 */
void rotation_matrix(double angle, double x, double y, double *rx, double *ry)
{
    *rx = x * cos(angle) - y * sin(angle);
    *ry = x * sin(angle) + y * cos(angle);
}

double degrees_to_rad(double degrees)
{
    return degrees * (3.14159265359 / 180);
}