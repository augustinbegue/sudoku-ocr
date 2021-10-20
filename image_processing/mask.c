#include "image.h"

/**
 * @brief Applies a mask on an image
 *
 * @param image_path path containing the image to apply the mask to
 * @param mask mask to apply - ⚠️ resolution must be >= than the image
 */
void apply_mask(Image *image, Image *mask)
{
    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            if (!mask->pixels[x][y].r)
            {
                image->pixels[x][y].r = image->pixels[x][y].g
                    = image->pixels[x][y].b = 255;
            }
        }
    }
}