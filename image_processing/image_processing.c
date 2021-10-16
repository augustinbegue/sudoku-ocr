#include <stdbool.h>
#include "blur.h"
#include "filters.h"
#include "helpers.h"
#include "image.h"
#include "mask.h"
#include "morph.h"

/**
 * @brief Processes the images contained in imagept and maskpt to create and
 * b&w image exploitable by an ocr algorithm.
 *
 * @param maskpt image mask to extract pixel from
 * @param imagept image to process
 * @param save_mask save the mask to a separate file
 * @param mask_output_path path where the mask is to be saved
 */
void process_image(
    Image *maskpt, Image *imagept, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("[1]♻️ Processing the image.\n");

    verbose_save(verbose_mode, verbose_path, "0-processing-step.png", imagept);

    /*
     * PASS 1 - Create a mask with the pixels to keep
     */

    if (verbose_mode)
        printf("   🖌  Applying grayscale and filtering noise.\n");

    // Grayscale and contrast adjustement
    filter_grayscale(maskpt, 0);

    filter_gamma(maskpt, 255);

    // Gaussian blur for noise removal
    gaussian_blur_image(maskpt, 11, 1, 1);

    if (verbose_mode)
        printf("   🎨 Average Color: %i\n", (int)maskpt->average_color);
    else
        fprintf(stderr, "\33[2K\r[===--------------------------]");

    verbose_save(verbose_mode, verbose_path, "1-processing-step.png", maskpt);

    if (maskpt->average_color >= 160)
    {
        filter_gamma(maskpt, 512);

        // Erosion and Dilation for further noise removal and character
        // enlargement
        morph(maskpt, Erosion, 3);

        morph(maskpt, Dilation, 5);
    }
    else // Too much black = imperfections -> we remove it by bulk
    {
        // Erosion and Dilation for further noise removal and character
        // enlargement
        morph(maskpt, Erosion, 9);

        morph(maskpt, Dilation, 9);
    }

    if (verbose_mode)
        printf("   🔲 Applying a mask threshold.\n");
    else
        fprintf(stderr, "\33[2K\r[=====------------------------]");

    // Mask creation from a dynamic threshold
    filter_threshold(maskpt);

    verbose_save(verbose_mode, verbose_path, "2-processing-step.png", maskpt);

    /*
     * PASS 2 - Apply the mask to a clean version of the image and reapply
     * processing
     */

    if (verbose_mode)
        printf("   📥 Applying the mask on the original image.\n");
    else
        fprintf(stderr, "\33[2K\r[=======----------------------]");

    // Apply the mask onto the clean image
    apply_mask(imagept, maskpt);

    // Adjust the colors of the obtained image
    filter_grayscale(imagept, 0);
    filter_contrast(imagept, 128);
    filter_gamma(imagept, 512);

    verbose_save(verbose_mode, verbose_path, "3-processing-step.png", imagept);

    if (verbose_mode)
        printf("   🔲 Applying a dynamic threshold.\n");
    else
        fprintf(stderr, "\33[2K\r[=========--------------------]");

    // Dynamic treshold to binarize the image
    filter_dynamic_threshold(imagept, 4);

    verbose_save(verbose_mode, verbose_path, "4-processing-step.png", imagept);

    if (verbose_mode)
        printf("   🖌  Filtering noise.\n");
    else
        fprintf(stderr, "\33[2K\r[===========-----------------]");

    if (verbose_mode)
        printf("   🎨 Average Color: %i\n", (int)imagept->average_color);

    // Special interval to target foggy images -> noise removal
    if (imagept->average_color >= 230 && imagept->average_color <= 240)
    {
        morph(imagept, Erosion, 8);

        verbose_save(
            verbose_mode, verbose_path, "4.1-processing-step.png", imagept);

        morph(imagept, Dilation, 8);

        verbose_save(
            verbose_mode, verbose_path, "4.2-processing-step.png", imagept);
    }

    if (verbose_mode)
        printf("   🔳 Inverting the image.\n");
    else
        fprintf(stderr, "\33[2K\r[===========------------------]");

    // Invert the binarization
    filter_invert(imagept, 0);

    if (!verbose_mode)
        fprintf(stderr, "\33[2K\r[=============----------------]");

    verbose_save(verbose_mode, verbose_path, "5-processing-step.png", imagept);
}