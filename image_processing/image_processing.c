#include <gtk/gtk.h>
#include <stdbool.h>
#include "blur.h"
#include "borders.h"
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
void image_processing_extract_grid(
    Image *maskpt, Image *imagept, bool verbose_mode, char *verbose_path)
{
    int adaptive_range
        = (maskpt->width > maskpt->height ? maskpt->height : maskpt->width)
          / 300;

    /*
     * PASS 1 - Create a mask with the pixels to keep
     */

    printf("[1]♻️ Processing the image.\n");
    verbose_save(
        verbose_mode, verbose_path, "0.0-processing-step.png", imagept);

    printf("   🖌  Applying grayscale.\n");

    // Grayscale and contrast adjustement
    filter_grayscale(maskpt, 0);

    verbose_save(verbose_mode, verbose_path, "0.1-mask-grayscale.png", maskpt);

    filter_gamma(maskpt, 255);

    verbose_save(verbose_mode, verbose_path, "0.2-mask-gamma.png", maskpt);

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

    printf("   🖌  Filtering noise.\n");

    // Gaussian blur for noise removal
    convolution(kernel, adaptive_range, imagept, imagept, false);
    free(kernel);

    verbose_save(verbose_mode, verbose_path, "4.2-image-blur.png", imagept);

    filter_gamma(maskpt, 255);
    filter_contrast(maskpt, 128);

    printf("   🧮  Normalizing colors.\n");

    verbose_save(
        verbose_mode, verbose_path, "4.3-image-gamma-contrast.png", imagept);

    // Dilate and erode to extract the important features
    morph(imagept, Erosion, adaptive_range);
    verbose_save(
        verbose_mode, verbose_path, "4.4-image-dilation.png", imagept);
    morph(imagept, Dilation, adaptive_range);
    verbose_save(verbose_mode, verbose_path, "4.5-image-erosion.png", imagept);

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

    // Median Filter on the image
    Image *blurred = malloc(sizeof(Image));
    *blurred = clone_image(input);
    filter_median(input, blurred, image_size / 100);

    verbose_save(verbose_mode, verbose_path, "9.2-blurred.png", blurred);

    // Dilation and Erosion
    morph(blurred, Dilation, image_size / 250);
    morph(blurred, Erosion, image_size / 250);

    verbose_save(
        verbose_mode, verbose_path, "9.3-erosion-dilation.png", blurred);

    // Adjusting colors
    filter_contrast(blurred, 64);
    filter_gamma(blurred, 255);

    filter_threshold(blurred);

    verbose_save(
        verbose_mode, verbose_path, "9.4-colors-adjusted.png", blurred);

    Image old = *input;
    *input = *blurred;
    free_Image(&old);
    free(blurred);
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

    // Searching for the most extreme coordinates of the remaining white
    // pixels
    bool changed = true;

    // Searching in a defined range
    int range = image_size / 8;

    // While the coordinates change, we continue to search
    // If the coordinates did not change between two iterations, this means
    // that we have detected the whole digit
    while (changed)
    {
        changed = false;

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

        // Increasing the range of the search to gradually get the whole digit
        range++;
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