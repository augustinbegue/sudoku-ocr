#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include "automatic_rotation.h"
#include "edge_averaging.h"
#include "edge_detection.h"
#include "geometry.h"
#include "helpers.h"
#include "hough_transform.h"
#include "image.h"
#include "image_processing.h"
#include "int_list.h"
#include "list.h"
#include "rotation.h"
#include "square_detection.h"
#include "square_selection.h"

static void print_help(const char *exec_name)
{
    printf("Usage: %s file [options]\n", exec_name);
    printf("Options:\n");
    printf("\t-o <file>     Save the output into <file>. (PNG format)\n");
    printf("\t-r <angle>    Rotate the image according to the specified "
           "<angle>.\n");
    printf("\t-m <file>     Save the mask image into <file>.  (PNG format)\n");
    printf("\t-v <path>     Verbose mode: save every step of the process in "
           "the folder <path>.\n");
    printf("\n");
    printf("For more information, see: "
           "https://github.com/augustinbegue/sudoku-ocr\n");
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        errx(1, "missing operand.\nTry --help for more information.");
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        // --help argument -> print help message
        print_help(argv[0]);
        return 0;
    }

    bool save_mask = false, image_rotation = false, verbose_mode = false;
    char *input_path = "", *output_path = "output.bmp",
         *mask_output_path = "output.grayscale.bmp", *verbose_path = "output";

    double rotation_amount = 0.0;

    // Argument handling
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            // next argument is the output_path
            i++;
            output_path = (char *)argv[i];
            continue;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            save_mask = true;
            // next argument is the mask_output_path
            i++;
            mask_output_path = (char *)argv[i];
            continue;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            image_rotation = true;
            // next argument is the rotation amount
            i++;
            rotation_amount = strtod(argv[i], 0);
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            verbose_mode = true;
            // next argument is the rotation amount
            i++;
            verbose_path = (char *)argv[i];
        }
        else
        {
            input_path = (char *)argv[i];
        }
    }

    // File loading and processing
    if (access(input_path, F_OK) == 0)
    {
        // Verbose mode folder creation
        if (verbose_mode)
        {
            struct stat st = {0};

            if (stat(verbose_path, &st) == -1)
            {
                _mkdir(verbose_path);
            }
        }

        /*
         * Loading Image
         */

        Image image = SDL_Surface_to_Image(load_image(input_path));
        Image *imagept = &image;
        Image mask = clone_image(imagept);
        Image *maskpt = &mask;

        if (!verbose_mode)
            fprintf(stderr, "\33[2K\r[=----------------------------]");

        /*
         * Processing
         */

        process_image(maskpt, imagept, verbose_mode, verbose_path);

        if (save_mask)
            save_image(Image_to_SDL_Surface(maskpt), mask_output_path);

        free_Image(maskpt);

        /*
         * Rotation
         */
        Image rotated_image;
        Image *rotated_imagept;

        if (image_rotation)
        {
            rotated_image = rotate_image(imagept, rotation_amount);
            rotated_imagept = &rotated_image;

            if (verbose_mode)
                verbose_save(verbose_mode, verbose_path, "5-rotated.png",
                    rotated_imagept);
        }
        else
        {
            rotated_imagept = imagept;
        }

        if (!verbose_mode)
            fprintf(stderr, "\33[2K\r[===============-------------]");

        /*
         * Edge detection
         */
        Image edge_image = canny_edge_filtering(
            rotated_imagept, verbose_mode, verbose_path);

        int_list *edges_x = li_create();
        int_list *edges_y = li_create();

        Image lines_image = clone_image(rotated_imagept);
        Image *lines_imagept = &lines_image;

        int **hough_accumulator = hough_transform(&edge_image, lines_imagept,
            edges_x, edges_y, verbose_mode, verbose_path);

        if (!verbose_mode)
            fprintf(stderr, "\33[2K\r[==========================--]");

        int edge_num = 0;
        int **edges = average_edges(edges_x, edges_y, lines_imagept,
            verbose_mode, verbose_path, &edge_num);

        if (!verbose_mode)
            fprintf(stderr, "\33[2K\r");
        else
            printf("   ⏹️ Finding squares...\n");

        list *squares = find_squares(edges, edge_num, lines_imagept);

        square *selected_square = select_square(
            squares, lines_imagept, verbose_mode, verbose_path);

        Image autorotated_image = automatic_rotation(hough_accumulator,
            selected_square, rotated_imagept, verbose_mode, verbose_path);

        verbose_save(verbose_mode, verbose_path, "8-autorotated.png",
            &autorotated_image);

        Image cropped_image = crop_image(&autorotated_image, selected_square);

        verbose_save(
            verbose_mode, verbose_path, "9-cropped.png", &cropped_image);

        // Saves the final image in the output_path file
        save_image(Image_to_SDL_Surface(&cropped_image), output_path);

        // Freeing
        free_Image(imagept); // Also frees rotated_imagept if there has been no
                             // rotation (they are the same)
        if (image_rotation)
            free_Image(rotated_imagept);
        free_Image(&edge_image);
        free_Image(&autorotated_image);
        free_Image(lines_imagept);
        free_Image(&cropped_image);

        free_2d_arr(edges, edge_num);

        l_free_values(squares);

        li_free(edges_x);
        li_free(edges_y);

        free(selected_square);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}