#include <err.h>
#include <stdio.h>
#include "../utils/helpers.h"
#include "../utils/image.h"

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

static void _contrast(Pixel *pixel, int value)
{
    pixel->r = _contrast_helper(pixel->r, value);
    pixel->g = _contrast_helper(pixel->g, value);
    pixel->b = _contrast_helper(pixel->b, value);
}

static void _bw(Pixel *pixel, int threshold)
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