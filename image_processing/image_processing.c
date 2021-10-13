#include <stdbool.h>
#include "../utils/image.h"
#include "blur.h"
#include "filters.h"
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
void process_image(Image *maskpt, Image *imagept)
{
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

    /*
     * PASS 2 - Apply the mask to a clean version of the image and reapply
     * processing
     */

    // Apply the mask onto the clean image
    apply_mask(imagept, maskpt);

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
}