#include <string.h>
#include "morph.h"
#include "helpers.h"
#include "image.h"

Pixel erode(Image *image, int mask_size, int x, int y)
{
    Uint8 min = image->pixels[x][y].r;

    int mid = mask_size / 2, irange = 0;
    for (int i = -1 * mid; i <= mid; i++, irange++)
    {
        int xx = clamp(x + i, 0, image->width - 1), jrange = 0;

        for (int j = -1 * mid; j <= mid; j++, jrange++)
        {
            int yy = clamp(y + j, 0, image->height - 1);

            Uint8 val = image->pixels[xx][yy].r;

            if (val < min)
            {
                min = val;
            }
        }
    }

    Pixel new_pixel;
    new_pixel.r = new_pixel.g = new_pixel.b = min;

    return new_pixel;
}

Pixel dilate(Image *image, int mask_size, int x, int y)
{
    Uint8 max = image->pixels[x][y].r;

    int mid = mask_size / 2, irange = 0;
    for (int i = -1 * mid; i <= mid; i++, irange++)
    {
        int xx = clamp(x + i, 0, image->width - 1), jrange = 0;

        for (int j = -1 * mid; j <= mid; j++, jrange++)
        {
            int yy = clamp(y + j, 0, image->height - 1);

            Uint8 val = image->pixels[xx][yy].r;

            if (val > max)
            {
                max = val;
            }
        }
    }

    Pixel new_pixel;
    new_pixel.r = new_pixel.g = new_pixel.b = max;

    return new_pixel;
}

/**
 * @brief Erodes or Dilates a grayscale image
 *
 * @param image image to morph
 * @param morph_type erode or dilate
 * @param mask_size size of the mask used for the morph
 * @returns morphed image
 */
void morph(Image *image, MorphType morph_type, int mask_size)
{
    double pnum = image->width * image->height;
    image->average_color = 0;

    Pixel **new_pixels = malloc(sizeof(Pixel *) * image->width);
    for (int i = 0; i < image->width; i++)
        new_pixels[i] = malloc(sizeof(Pixel) * image->height);

    if (mask_size % 2 == 0)
        mask_size++;

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            switch (morph_type)
            {
                case Dilation:
                    new_pixels[x][y] = dilate(image, mask_size, x, y);
                    break;
                case Erosion:
                    new_pixels[x][y] = erode(image, mask_size, x, y);
                    break;
                default:
                    break;
            }

            double c_avg = (image->pixels[x][y].r + image->pixels[x][y].g
                               + image->pixels[x][y].b)
                           / 3;

            image->average_color += c_avg / pnum;
        }
    }

    for (int i = 0; i < image->width; i++)
        free(image->pixels[i]);
    free(image->pixels);

    image->pixels = new_pixels;
}
