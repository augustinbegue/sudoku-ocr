#include "image.h"
#include "morph.h"

void remove_cell_borders(Image *input)
{
    int image_size = input->width;
    int center = image_size / 2;

    int left_border_width = 0;
    int right_border_width = 0;
    int top_border_height = 0;
    int bottom_border_height = 0;

    Image border_sampling = clone_image(input);

    // Dilate image to make borders thicker
    morph(&border_sampling, Dilation, image_size / 10);

    int border_size_threshold = image_size / 8;
    for (int x = 0; x < image_size; x++)
    {
        for (int y = 0; y < image_size; y++)
        {
            // Potentially a border
            if (border_sampling.pixels[x][y].r == 255)
            {
                int border_size = 0;
                bool border = true;

                // Test if the pixel is on a horizontal border
                int blackpixels = 0;
                for (int xx = 0; xx < image_size; xx++)
                    if (border_sampling.pixels[xx][y].r == 0)
                        blackpixels++;

                if (blackpixels > border_size_threshold)
                    border = false;

                if (!border)
                {
                    border = true;

                    // Test if the pixel is on a vertical border
                    blackpixels = 0;
                    for (int yy = 0; yy < image_size; yy++)
                        if (border_sampling.pixels[x][yy].r == 0)
                            blackpixels++;

                    if (blackpixels > border_size_threshold)
                        border = false;

                    if (border)
                    {
                        // left/right border
                        if (x > center)
                        {
                            // right border
                            border_size = image_size - x;

                            if (border_size > right_border_width)
                                right_border_width = border_size;
                        }
                        else
                        {
                            // left border
                            border_size = x;

                            if (border_size > left_border_width)
                                left_border_width = border_size;
                        }
                    }
                }
                else
                {
                    // top/bottom border
                    if (y > center)
                    {
                        // bottom border
                        border_size = image_size - y;

                        if (border_size > bottom_border_height)
                            bottom_border_height = border_size;
                    }
                    else
                    {
                        // top border
                        border_size = y;

                        if (border_size > top_border_height)
                            top_border_height = border_size;
                    }
                }
            }
        }
    }

    free_Image(&border_sampling);

    // Removing borders
    for (int y = 0; y < image_size; y++)
    {
        for (int x = 0; x <= left_border_width; x++)
        {
            input->pixels[x][y].r = 0;
            input->pixels[x][y].g = 0;
            input->pixels[x][y].b = 0;
        }
        for (int x = image_size - right_border_width; x < image_size; x++)
        {
            input->pixels[x][y].r = 0;
            input->pixels[x][y].g = 0;
            input->pixels[x][y].b = 0;
        }
    }

    for (int x = 0; x < image_size; x++)
    {
        for (int y = 0; y <= top_border_height; y++)
        {
            input->pixels[x][y].r = 0;
            input->pixels[x][y].g = 0;
            input->pixels[x][y].b = 0;
        }

        for (int y = image_size - bottom_border_height; y < image_size; y++)
        {
            input->pixels[x][y].r = 0;
            input->pixels[x][y].g = 0;
            input->pixels[x][y].b = 0;
        }
    }
}
