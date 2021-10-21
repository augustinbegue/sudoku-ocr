#include <stdbool.h>
#include "helpers.h"
#include "image.h"

#define CELL_NUMBER_ROW 9

Image **split_grid(Image *input, square *selected_square, bool verbose_mode,
    char *verbose_path)
{
    Image cropped_image = crop_image(input, selected_square);

    verbose_save(
        verbose_mode, verbose_path, "9.0-cropped.png", &cropped_image);

    int cropped_size = cropped_image.width;
    int cell_size = cropped_size / 9;

    printf("Cell Size: %i\n", cell_size);

    /*
     * Array containing the 9*9 grid cells
     * Grid cell x,y can be accessed by image_cells[x * 9 + y]
     */
    Image **image_cells
        = malloc(sizeof(Image *) * CELL_NUMBER_ROW * CELL_NUMBER_ROW + 1);

    for (int i = 0; i < CELL_NUMBER_ROW; i++)
        for (int j = 0; j < CELL_NUMBER_ROW; j++)
            image_cells[i * CELL_NUMBER_ROW + j] = NULL;

    for (int cell_x = 0; cell_x < 9; cell_x++)
    {
        for (int cell_y = 0; cell_y < 9; cell_y++)
        {
            int c = cell_x * CELL_NUMBER_ROW + cell_y;
            image_cells[c] = create_image(cell_size, cell_size);

            int start_x = cell_x * cell_size;
            int start_y = cell_y * cell_size;
            int end_x = (cell_x + 1) * cell_size;
            int end_y = (cell_y + 1) * cell_size;

            for (int x = start_x, out_x = 0; x < end_x && out_x < cell_size;
                 x++, out_x++)
            {
                for (int y = start_y, out_y = 0;
                     y < end_y && out_y < cell_size; y++, out_y++)
                {
                    Pixel in = cropped_image.pixels[x][y];
                    image_cells[c]->pixels[out_x][out_y] = in;
                }
            }
        }
    }

    if (verbose_mode)
    {
        for (int x = 0; x < 9; x++)
        {
            for (int y = 0; y < 9; y++)
            {
                char file_name[200];
                snprintf(file_name, 200, "9.1-grid-%ix%i.png", x + 1, y + 1);

                verbose_save(verbose_mode, verbose_path, file_name,
                    image_cells[x * CELL_NUMBER_ROW + y]);
            }
        }
    }

    free_Image(&cropped_image);

    return image_cells;
}