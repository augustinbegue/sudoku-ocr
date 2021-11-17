#include <stdbool.h>
#include "blur.h"
#include "filters.h"
#include "helpers.h"
#include "image.h"
#include "morph.h"
#include "sauvola.h"

#define CELL_NUMBER_ROW 9
#define CELL_SIZE_PIXELS 28

Image **split_grid(Image *input, bool verbose_mode, char *verbose_path)
{
    int image_size = input->width;
    int original_cell_size = image_size / CELL_NUMBER_ROW;
    int cell_size = CELL_SIZE_PIXELS;
    int downscale_factor = original_cell_size / cell_size;

    int adaptive_range = 2;

    // Grayscaling the image
    filter_grayscale(input, 0);

    // Blurring the image
    double *kernel = get_gaussian_smoothing_kernel(image_size / 200, 1.5);
    convolution(kernel, image_size / 200, input, input, false);
    free(kernel);

    // Adjusting colors
    filter_gamma(input, 255);

    /*
     * Array containing the 9*9 grid cells
     * Grid cell x,y can be accessed by image_cells[x * 9 + y]
     */
    Image **image_cells
        = malloc(sizeof(Image *) * CELL_NUMBER_ROW * CELL_NUMBER_ROW + 1);

    for (int i = 0; i < CELL_NUMBER_ROW; i++)
        for (int j = 0; j < CELL_NUMBER_ROW; j++)
            image_cells[i * CELL_NUMBER_ROW + j] = NULL;

    int x = 0;
    int y = 0;
    for (int cell_x = 0; cell_x < 9; cell_x++)
    {
        for (int cell_y = 0; cell_y < 9; cell_y++)
        {
            int c = cell_x * CELL_NUMBER_ROW + cell_y;
            Image *cell = create_image(cell_size, cell_size);
            image_cells[c] = cell;

            for (int cell_x = 0; cell_x < cell_size; cell_x++)
            {
                for (int cell_y = 0; cell_y < cell_size; cell_x++)
                {
                    Pixel in;

                    // TODO: Downscale algorithm

                    image_cells[c]->pixels[cell_x][cell_x] = in;
                }
            }

            // Morphological operations to remove noise
            morph(image_cells[c], Erosion, 2 * adaptive_range);
            morph(image_cells[c], Dilation, adaptive_range);

            // Thresholding of the cell to binarize it
            filter_sauvola(image_cells[c], image_cells[c], adaptive_range, 0.2,
                false, "");

            // Inverting the cell to make it follow the neural network's
            // convention
            filter_invert(image_cells[c], 0);
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