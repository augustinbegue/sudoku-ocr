#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "image_processing.h"
#include "math.h"

#define CELL_NUMBER_ROW 9
#define CELL_SIZE_PIXELS 28

Image **split_grid(Image *input, bool verbose_mode, char *verbose_path)
{
    image_processing_extract_digits(input, verbose_mode, verbose_path);

    /*
     * Array containing the 9*9 grid cells
     * Grid cell x,y can be accessed by image_cells[x * 9 + y]
     */
    Image **image_cells
        = malloc(sizeof(Image *) * CELL_NUMBER_ROW * CELL_NUMBER_ROW + 1);

    for (int i = 0; i < CELL_NUMBER_ROW; i++)
        for (int j = 0; j < CELL_NUMBER_ROW; j++)
            image_cells[i * CELL_NUMBER_ROW + j] = NULL;

    int image_size = input->width;
    int original_cell_size = image_size / 9;

    for (int cell_x = 0; cell_x < 9; cell_x++)
    {
        for (int cell_y = 0; cell_y < 9; cell_y++)
        {
            int c = cell_x * CELL_NUMBER_ROW + cell_y;
            Image *large_cell
                = create_image(original_cell_size, original_cell_size);

            int start_x = cell_x * original_cell_size;
            int start_y = cell_y * original_cell_size;
            int end_x = (cell_x + 1) * original_cell_size;
            int end_y = (cell_y + 1) * original_cell_size;

            for (int x = start_x, out_x = 0;
                 x < end_x && out_x < original_cell_size; x++, out_x++)
            {
                for (int y = start_y, out_y = 0;
                     y < end_y && out_y < original_cell_size; y++, out_y++)
                {
                    Pixel in = input->pixels[x][y];
                    large_cell->pixels[out_x][out_y] = in;
                }
            }

            square boundaries
                = image_processing_detect_digit_boundaries(large_cell);

            Image *cropped;
            if (boundaries.c1.x < boundaries.c2.x)
            {
                cropped = crop_image(large_cell, &boundaries);
                free_Image(large_cell);
                free(large_cell);
            }
            else
                cropped = large_cell;

            image_cells[c] = downscale_image(cropped, CELL_SIZE_PIXELS);

            free_Image(cropped);
            free(cropped);
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