#include <err.h>
#include <math.h>
#include <stdio.h>
#include "../utils/helpers.h"
#include "../utils/image.h"
#include "./threshold.h"

static void _grayscale(Pixel *pixel, int brightness)
{
    pixel->r = pixel->g = pixel->b
        = (0.3 * pixel->r + 0.59 * pixel->g + 0.11 * pixel->b) + brightness;
}

static double _contrast_helper(Uint8 p, int contrast)
{
    double c = clamp(contrast, 0, 128);
    double factor = (259 * (c + 255)) / (255 * (259 - c));

    return clamp(factor * (p - 128) + 128, 0, 255);
}

static double _gamma_helper(Uint8 p, int gamma)
{
    double c = gamma / 128;
    double factor = pow(p / 255.0, c);

    return clamp(factor * 255.0, 0, 255);
}

static void _contrast(Pixel *pixel, int value)
{
    pixel->r = _contrast_helper(pixel->r, value);
    pixel->g = _contrast_helper(pixel->g, value);
    pixel->b = _contrast_helper(pixel->b, value);
}

static void _gamma(Pixel *pixel, int value)
{
    pixel->r = _gamma_helper(pixel->r, value);
    pixel->g = _gamma_helper(pixel->g, value);
    pixel->b = _gamma_helper(pixel->b, value);
}

static void _bw(Pixel *pixel, int threshold)
{
    pixel->r = pixel->g = pixel->b = pixel->r > threshold ? 255 : 0;
}

static void _bw_mask(Pixel *pixel, int threshold)
{
    pixel->r = pixel->g = pixel->b = pixel->r > threshold ? 0 : 255;
}

void filter_grayscale(Image *image, double brightness)
{
    image_filter(image, _grayscale, brightness);
}

void filter_bw(Image *image, double threshold)
{
    image_filter(image, _bw, threshold);
}

void filter_contrast(Image *image, double value)
{
    image_filter(image, _contrast, value);
}

void filter_gamma(Image *image, double value)
{
    image_filter(image, _gamma, value);
}

/**
 * @brief applies a dynamic black and white threshold on image in n.
 * subdivisions
 *
 * @param image image to apply the filter on
 * @param subdivisions number of subdivisions to do the filtering
 */
void filter_dynamic_threshold(Image *image, int subdivisions)
{
    int subdivisions_x = subdivisions;
    int subdivisions_y = subdivisions;

    subdivisions = subdivisions_x * subdivisions_y;

    int divx = image->height / subdivisions_x;
    int divy = image->width / subdivisions_y;

    int divnum = 1;
    for (int divnum_x = 0; divnum_x < subdivisions_x; divnum_x++)
    {
        int startx = divx * divnum_x, endx = divx * (divnum_x + 1);

        if (divnum_x + 1 == subdivisions_x)
            endx = image->width;

        for (int divnum_y = 0; divnum_y < subdivisions_y; divnum_y++, divnum++)
        {
            int starty = divy * divnum_y, endy = divy * (divnum_y + 1);

            if (divnum_y + 1 == subdivisions_y)
                endy = image->height;

            int *hist
                = image_grayscale_histogram(image, startx, endx, starty, endy);

            Uint8 threshold = get_histogram_threshold(hist);

            printf("...✂️ Subdivision n.%i/%i: %d -> %d * %d -> %d | "
                   "threshold: %i\n",
                divnum, subdivisions, startx, endx, starty, endy, threshold);

            image_partial_filter(
                image, _bw_mask, threshold, startx, endx, starty, endy);

            free(hist);
        }
    }
}

/**
 * @brief Applies a black and white threshold on a image
 *
 * @param image image to apply the filter on
 */
void filter_threshold(Image *image)
{
    int height = image->height;
    int width = image->width;

    int *hist = image_grayscale_histogram(image, 0, width, 0, height);

    Uint8 threshold = get_histogram_threshold(hist);

    image_filter(image, _bw, threshold);

    free(hist);
}