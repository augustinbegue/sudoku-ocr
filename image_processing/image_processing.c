#include <gtk/gtk.h>
#include <stdbool.h>
#include "blur.h"
#include "filters.h"
#include "helpers.h"
#include "image.h"
#include "mask.h"
#include "morph.h"
#include "sauvola.h"

/**
 * @brief Processes the images contained in imagept and maskpt to create and
 * b&w image exploitable by an ocr algorithm.
 *
 * @param maskpt image mask to extract pixel from
 * @param imagept image to process
 * @param verbose_mode
 * @param verbose_path
 */
void image_processing_extract_grid(Image *maskpt, Image *imagept,
    bool verbose_mode, char *verbose_path, bool gtk)
{
    int adaptive_range
        = (maskpt->width > maskpt->height ? maskpt->height : maskpt->width)
          / 300;

    /*
     * PASS 1 - Create a mask with the pixels to keep
     */

    if (verbose_mode)
        printf("[1]♻️ Processing the image.\n");
    verbose_save(
        verbose_mode, verbose_path, "0.0-processing-step.png", imagept);

    if (verbose_mode)
        printf("   🖌  Applying grayscale.\n");

    // Grayscale and contrast adjustement
    filter_grayscale(maskpt, 0);

    verbose_save(verbose_mode, verbose_path, "0.1-mask-grayscale.png", maskpt);

    filter_gamma(maskpt, 255);

    verbose_save(verbose_mode, verbose_path, "0.2-mask-gamma.png", maskpt);

    if (verbose_mode)
        printf("   🖌  Filtering noise.\n");

    // Gaussian blur for noise removal
    double *kernel = get_gaussian_smoothing_kernel(adaptive_range, 1.5);
    convolution(kernel, adaptive_range, maskpt, maskpt, false);

    verbose_save(verbose_mode, verbose_path, "1-mask-blur.png", maskpt);

    // Dilate and erode to extract the important features
    morph(imagept, Erosion, adaptive_range);
    verbose_save(verbose_mode, verbose_path, "1.1-mask-dilation.png", imagept);
    morph(imagept, Dilation, adaptive_range);
    verbose_save(verbose_mode, verbose_path, "1.2-mask-erosion.png", imagept);

    if (verbose_mode)
        printf("   🔲 Applying a mask threshold.\n");

    // Sauvola thresholding
    filter_sauvola(maskpt, maskpt, adaptive_range + adaptive_range, 0.2,
        verbose_mode, verbose_path);

    verbose_save(
        verbose_mode, verbose_path, "2.3-sauvola-thresholding.png", maskpt);

    /*
     * PASS 2 - Apply the mask to a clean version of the image and reapply
     * processing
     */
    if (verbose_mode)
        printf("   📥 Applying the mask on the original image.\n");

    // Apply the mask onto the clean image
    apply_mask(imagept, maskpt);

    // Adjust the colors of the obtained image
    filter_grayscale(imagept, 0);

    verbose_save(verbose_mode, verbose_path, "3-mask-applied.png", imagept);

    // Grayscale and contrast adjustement
    filter_grayscale(maskpt, 0);

    verbose_save(
        verbose_mode, verbose_path, "4.1-image-grayscale.png", imagept);

    if (verbose_mode)
        printf("   🖌  Filtering noise.\n");

    // Gaussian blur for noise removal
    convolution(kernel, adaptive_range, imagept, imagept, false);
    free(kernel);

    verbose_save(verbose_mode, verbose_path, "4.2-image-blur.png", imagept);

    filter_gamma(maskpt, 255);
    filter_contrast(maskpt, 128);

    if (verbose_mode)
        printf("   🧮  Normalizing colors.\n");

    verbose_save(
        verbose_mode, verbose_path, "4.3-image-gamma-contrast.png", imagept);

    // Dilate and erode to extract the important features
    morph(imagept, Erosion, adaptive_range);
    verbose_save(
        verbose_mode, verbose_path, "4.4-image-dilation.png", imagept);
    morph(imagept, Dilation, adaptive_range);
    verbose_save(verbose_mode, verbose_path, "4.5-image-erosion.png", imagept);

    if (verbose_mode)
        printf("   🎨 Average Color: %i\n", (int)imagept->average_color);

    if ((int)imagept->average_color == 237)
    {
        filter_grayscale(imagept, -adaptive_range);
        filter_normalize(imagept);
        verbose_save(
            verbose_mode, verbose_path, "4.6-image-normalized.png", imagept);
    }
    else if ((int)imagept->average_color == 170)
    {
        filter_gamma(maskpt, 512);
        filter_contrast(maskpt, 255);
    }

    if (verbose_mode)
        printf("   🔲  Binarizing the image.\n");

    // Adaptive threshold to binarise
    filter_threshold(imagept);

    verbose_save(verbose_mode, verbose_path, "5-binarized.png", imagept);
}

/**
 * @brief Processes the images contained in input and extracts the digits
 * contained in the grid
 *
 * @param input
 * @param verbose_mode
 * @param verbose_path
 */
void image_processing_extract_digits(
    Image *input, bool verbose_mode, char *verbose_path)
{
    int image_size = input->width;

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
}

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

/**
 * @brief Detects the boundaries of the digit countained in the black and white
 * image
 *
 * @ref Algorithm description:
 * We start by removing the white borders on the image.
 * Then, we go trough the image and look for the most extreme coordinates of
 * the remaining white pixels
 *
 *
 * @param input input image where the digit is located
 * @returns the boundaries of the digit
 */
square image_processing_detect_digit_boundaries(Image *input)
{
    int image_size = input->width;
    int center = image_size / 2;
    int min_x = image_size;
    int max_x = 0;
    int min_y = image_size;
    int max_y = 0;

    // Detecting the white borders of the cells
    remove_cell_borders(input);

    bool changed = true;
    int range = image_size / 8;

    while (changed)
    {
        changed = false;

        // Searching for the most extreme coordinates of the remaining white
        // pixels
        int start_x = center - range;
        int end_x = center + range;
        int start_y = center - range;
        int end_y = center + range;

        if (start_x < 0)
            start_x = 0;
        if (end_x >= image_size)
            end_x = image_size - 1;

        if (start_y < 0)
            start_y = 0;
        if (end_y >= image_size)
            end_y = image_size - 1;

        for (int x = start_x; x <= end_x; x++)
        {
            for (int y = start_y; y <= end_y; y++)
            {
                if (input->pixels[x][y].g == 255)
                {
                    if (x < min_x)
                    {
                        min_x = x;
                        changed = true;
                    }
                    if (x > max_x)
                    {
                        max_x = x;
                        changed = true;
                    }
                    if (y < min_y)
                    {
                        min_y = y;
                        changed = true;
                    }
                    if (y > max_y)
                    {
                        max_y = y;
                        changed = true;
                    }
                }
            }
        }

        range += image_size / 50;
    }

    square boundaries;

    boundaries.c1.x = min_x;
    boundaries.c1.y = min_y;

    boundaries.c2.x = max_x;
    boundaries.c2.y = min_y;

    boundaries.c3.x = max_x;
    boundaries.c3.y = max_y;

    boundaries.c4.x = min_x;
    boundaries.c4.y = max_y;

    return boundaries;
}