#include <err.h>
#include <math.h>
#include <stdio.h>
#include "helpers.h"
#include "image.h"
#include "threshold.h"

static void _grayscale(Pixel *pixel, int brightness)
{
    pixel->r = pixel->g = pixel->b
        = (0.40 * pixel->r + 0.35 * pixel->g + 0.25 * pixel->b) + brightness;
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

static void _bw_inverted(Pixel *pixel, int threshold)
{
    pixel->r = pixel->g = pixel->b = pixel->r > threshold ? 0 : 255;
}

static void _bw_invert(Pixel *pixel, int min)
{
    pixel->r = pixel->g = pixel->b = pixel->r >= 128 ? 0 + min : 255;
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

void filter_invert(Image *image, double value)
{
    image_filter(image, _bw_invert, value);
}

/**
 * @brief applies a dynamic black and white threshold on image in n.
 * subdivisions
 *
 * @param image image to apply the filter on
 * @param radius radius of the pixel window to take the values from
 */
void filter_dynamic_threshold(Image *image, int radius)
{
    int width = image->width;
    int height = image->height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int colorSum = 0, colorMean = 0, sampleNum = 0, threshold = 0;

            for (int xi = x - radius; xi <= x + radius; xi++)
            {
                if (xi < 0 || xi >= width)
                    continue;

                for (int yi = y - radius; yi < y + radius; yi++)
                {
                    if (yi < 0 || yi >= height)
                        continue;

                    colorSum += image->pixels[xi][yi].r;
                    sampleNum++;
                }
            }

            colorMean = colorSum / sampleNum;
            threshold = colorMean - 2;

            Pixel *pix = &image->pixels[x][y];
            if (pix->r > threshold)
                pix->r = pix->g = pix->b = 255;
            else
                pix->r = pix->g = pix->b = 0;
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

    image_filter(image, _bw_inverted, threshold);

    free(hist);
}