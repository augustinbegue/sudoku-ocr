#include <stdbool.h>
#include "image.h"

Image *split_grid(Image *input, square *selected_square, bool verbose_mode,
    char *verbose_path)
{
    Image cropped_image = crop_image(&input, selected_square);

    verbose_save(verbose_mode, verbose_path, "9-cropped.png", &cropped_image);

    int size = cropped_image.width;
    int cell_size = size / 9;

    /*
     * Array containing the 9*9 grid cells
     * Grid cell x,y can be accessed by image_cells[x * 9 + y]
     */
    Image *image_cells = malloc(sizeof(Image) * 9 * 9 + 1);

    for (int cell_x = 0; cell_x < 9; cell_x++)
    {
        for (int cell_y = 0; cell_y < 9; cell_y++)
        {
            Image cell = image_cells[cell_x * size + cell_y];
            cell = create_image(size, size);

            int start_x = cell_x * size;
            int start_y = cell_y * size;
            int end_x = (cell_x + 1) * size;
            int end_y = (cell_y + 1) * size;
            int out_x = 0, out_y = 0;

            for (int x = start_x; x < end_x; x++, out_x++)
            {
                for (int y = start_y; y < end_y; y++, out_y++)
                {
                    cell.pixels[out_x][out_y] = input->pixels[x][y];
                }
            }
        }
    }

    return image_cells;
}