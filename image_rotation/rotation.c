#include <math.h>
#include "../utils/image.h"

/**
 * @brief Performs a rotation on the coordinates (x, y) by an angle
 *
 * @param angle degrees
 * @param x
 * @param y
 * @param center_x center of the image on the x axis
 * @param center_y center of the image on the y axis
 * @param rx rotated x coordinate
 * @param ry rotated y coordinate
 */
void corrected_rotation_matrix(double angle, double x, double y,
    double center_x, double center_y, double *rx, double *ry)
{
    *rx = (x - center_x) * cos(angle) - (y - center_y) * sin(angle) + center_x;
    *ry = (x - center_x) * sin(angle) + (y - center_y) * cos(angle) + center_y;
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
    printf("   🔃 Rotating image by %.0f° (%.5f rad)\n", angle,
        degrees_to_rad(angle));

    angle = degrees_to_rad(angle);

    int w = image->width;
    int h = image->height;
    double center_x = (w / (double)2);
    double center_y = (h / (double)2);

    Pixel black = {0, 0, 0};

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            double rx, ry;
            corrected_rotation_matrix(
                angle, (double)x, (double)y, center_x, center_y, &rx, &ry);

            if (0 <= rx && rx < w && 0 <= ry && ry < h)
            {
                Pixel org = image->pixels[(int)rx][(int)ry];
                Pixel dest = {org.r, org.g, org.b};
                rotated_image.pixels[x][y] = dest;
            }
            else
            {
                rotated_image.pixels[x][y] = black;
            }
        }
    }

    return rotated_image;
}
