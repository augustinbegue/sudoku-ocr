#include <stdbool.h>
#include "blur.h"
#include "filters.h"
#include "helpers.h"
#include "image.h"
#include "math.h"
#include "morph.h"
#include "sauvola.h"

#define CELL_NUMBER_ROW 9
#define CELL_SIZE_PIXELS 28

Image **split_grid(Image *input, bool verbose_mode, char *verbose_path)
{
    int image_size = input->width;
    double original_cell_size = image_size / (double)CELL_NUMBER_ROW;
    double cell_size = CELL_SIZE_PIXELS;
    double downscale_factor = original_cell_size / cell_size;

    // Grayscaling the image
    filter_grayscale(input, 0);

    // Blurring the image
    double *kernel = get_gaussian_smoothing_kernel(image_size / 100, 1.5);
    convolution(kernel, image_size / 100, input, input, false);
    free(kernel);

    verbose_save(verbose_mode, verbose_path, "9.2-blurred.png", input);

    // Dilation and Erosion
    morph(input, Dilation, image_size / 250);
    morph(input, Erosion, image_size / 250);

    verbose_save(
        verbose_mode, verbose_path, "9.3-erosion-dilation.png", input);

    // Adjusting colors
    filter_contrast(input, 64);
    filter_gamma(input, 255);

    filter_threshold(input);

    verbose_save(verbose_mode, verbose_path, "9.4-colors-adjusted.png", input);

    /*
     * Array containing the 9*9 grid cells
     * Grid cell x,y can be accessed by image_cells[x * 9 + y]
     */
    Image **image_cells
        = malloc(sizeof(Image *) * CELL_NUMBER_ROW * CELL_NUMBER_ROW + 1);

    for (int i = 0; i < CELL_NUMBER_ROW; i++)
        for (int j = 0; j < CELL_NUMBER_ROW; j++)
            image_cells[i * CELL_NUMBER_ROW + j] = NULL;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int c = i * CELL_NUMBER_ROW + j;
            Image *cell = create_image(cell_size, cell_size);
            image_cells[c] = cell;

            double x = i * original_cell_size;
            for (int cell_x = 0; cell_x < cell_size && x < image_size;
                 cell_x++)
            {
                double y = j * original_cell_size;
                for (int cell_y = 0; cell_y < cell_size && y < image_size;
                     cell_y++)
                {
                    int color = 0;

                    double pnum = 0;
                    for (int f = 0;
                         f < downscale_factor && (x + f) < image_size; f++)
                    {
                        for (int g = 0;
                             g < downscale_factor && (y + g) < image_size; g++)
                        {
                            int xx = x + f;
                            int yy = y + g;

                            color += input->pixels[xx][yy].r;

                            pnum++;
                        }
                    }

                    color /= pnum;

                    Pixel pix = {color, color, color};

                    image_cells[c]->pixels[cell_x][cell_y] = pix;

                    y += downscale_factor;
                }

                x += downscale_factor;
            }

            // Inverting the cell to make it follow the neural network's
            // convention
            // filter_invert(image_cells[c], 0);
        }
    }

    if (verbose_mode)
    {
        for (int x = 0; x < 9; x++)
        {
            for (int y = 0; y < 9; y++)
            {
                char file_name[200];
                snprintf(file_name, 200, "10-grid-%ix%i.png", x + 1, y + 1);

                verbose_save(verbose_mode, verbose_path, file_name,
                    image_cells[x * CELL_NUMBER_ROW + y]);
            }
        }
    }

    return image_cells;
}