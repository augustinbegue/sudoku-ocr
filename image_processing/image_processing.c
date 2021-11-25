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
 * @param save_mask save the mask to a separate file
 * @param mask_output_path path where the mask is to be saved
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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

    // Gaussian blur for noise removal
    convolution(kernel, adaptive_range, imagept, imagept, false);
    free(kernel);

    verbose_save(verbose_mode, verbose_path, "4.2-image-blur.png", imagept);

    filter_gamma(maskpt, 255);
    filter_contrast(maskpt, 128);

    if (verbose_mode)
        printf("   🧮  Normalizing colors.\n");
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

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
    if (gtk)
        while (gtk_events_pending())
            gtk_main_iteration();

    // Adaptive threshold to binarise
    filter_threshold(imagept);

    verbose_save(verbose_mode, verbose_path, "5-binarized.png", imagept);
}