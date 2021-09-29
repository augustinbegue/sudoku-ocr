#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "image_processing/blur.h"
#include "image_processing/filters.h"
#include "image_processing/mask.h"
#include "image_processing/morph.h"
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

    bool save_mask = false, rotate_image = false;
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
            rotate_image = true;
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

        Image mask = SDL_Surface_to_Image(load_image(input_path));
        Image *maskpt = &mask;

        /*
         * PREPROCESSING
         */

        /*
         * Image Rotation
         */
        if (rotate_image)
        {
            printf("...🔃 Rotating image by %.2f°\n", rotation_amount);
        }

        /*
         * PASS 1 - Create a mask with the pixels to keep
         */

        // Grayscale and contrast adjustement
        filter_grayscale(maskpt, 0);

        filter_gamma(maskpt, 255);

        // Gaussian blur for noise removal
        gaussian_blur_image(maskpt, 5, 1, 1);

        printf("...🎨 Average Color: %i\n", (int)maskpt->average_color);

        if (maskpt->average_color >= 170)
        {
            filter_contrast(maskpt, 128);

            // Erosion and Dilation for further noise removal and character
            // enlargement
            morph(maskpt, Erosion, 3);

            morph(maskpt, Dilation, 5);
        }
        else
        {
            // Erosion and Dilation for further noise removal and character
            // enlargement
            morph(maskpt, Erosion, 9);

            morph(maskpt, Dilation, 9);
        }

        // Mask creation from a dynamic threshold
        filter_dynamic_threshold(maskpt, 1);

        if (save_mask)
            save_image(Image_to_SDL_Surface(maskpt), mask_output_path);

        /*
         * PASS 2 - Apply the mask to a clean version of the image and reapply
         * processing
         */
        Image image = SDL_Surface_to_Image(load_image(input_path));
        Image *imagept = &image;

        // Apply the mask onto the clean image
        apply_mask(imagept, maskpt);

        // Mask is no longer needed and therefore freed
        free_Image(maskpt);

        filter_grayscale(imagept, 0);

        filter_gamma(imagept, 255);

        printf("...🎨 Average Color: %i\n", (int)maskpt->average_color);

        if (maskpt->average_color > 200)
        {
            gaussian_blur_image(imagept, 5, 2, 1);

            filter_contrast(imagept, 128);

            filter_gamma(imagept, 384);
        }

        filter_threshold(imagept);

        // save the image in the output_path file
        save_image(Image_to_SDL_Surface(imagept), output_path);

        free_Image(imagept);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}
