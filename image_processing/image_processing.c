#include <stdbool.h>
#include "../utils/image.h"
#include "blur.h"
#include "filters.h"
#include "mask.h"
#include "morph.h"

void process_image(
    Image *maskpt, Image *imagept, bool save_mask, char *mask_output_path)
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

    if (save_mask)
        save_image(Image_to_SDL_Surface(maskpt), mask_output_path);

    /*
     * PASS 2 - Apply the mask to a clean version of the image and reapply
     * processing
     */

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
}