#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "./image_processing/image_processing.h"
#include "./image_rotation/rotation.h"
#include "utils/image.h"

static void print_help(const char *exec_name)
{
    printf("Usage: %s [options] file ...\n", exec_name);
    printf("Options:\n");
    printf("\t-o <file>     Save the output into <file>.\n");
    printf("\t-m            Save the mask image into <file>.\n");
    printf("\t-r <angle>    Rotate the image according to the specified "
           "<angle>.\n");
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

    bool save_mask = false, image_rotation = false;
    char *input_path = "", *output_path = "./output.bmp",
         *mask_output_path = "./output.grayscale.bmp";

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
        else
        {
            input_path = (char *)argv[i];
        }
    }

    // File loading and processing
    if (access(input_path, F_OK) == 0)
    {
        /*
         * Loading Image
         */

        Image image = SDL_Surface_to_Image(load_image(input_path));
        Image *imagept = &image;
        Image mask = clone_image(imagept);
        Image *maskpt = &mask;

        /*
         * Processing
         */

        process_image(maskpt, imagept);

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
        }
        else
        {
            rotated_imagept = imagept;
        }

        // save the image in the output_path file
        save_image(Image_to_SDL_Surface(rotated_imagept), output_path);

        free_Image(imagept); // Also frees rotated_imagept if there has been no
                             // rotation (they are the same)
        if (image_rotation)
            free_Image(rotated_imagept);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}
