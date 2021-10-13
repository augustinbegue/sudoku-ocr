#include <math.h>
#include "../utils/image.h"

/**
 * @brief Performs a rotation on the coordinates (x, y) by an angle
 *
 * @param angle degrees
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

/**
 * @brief Rotates image counterclockwise by an angle
 *
 * @param image image to rotate
 * @param angle angle of the rotation in degrees
 */
Image rotate_image(Image *image, double angle)
{
    Image rotated_image = clone_image(image);
    printf("...🔃 Rotating image by %.2f°\n", angle);

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            double rx, ry;
            rotation_matrix(angle, (double)x, (double)y, &rx, &ry);

            // printf("%i, %i --> %f, %f\n", x, y, rx, ry);

            if (0 >= rx && 0 >= ry && rx < image->width && ry < image->height)
            {
                Pixel org = image->pixels[x][y];
                Pixel dest = {org.r, org.g, org.b};
                rotated_image.pixels[(int)rx][(int)ry] = dest;
            }
        }
    }

    return rotated_image;
}
