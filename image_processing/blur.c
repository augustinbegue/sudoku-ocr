#include <math.h>
#include <stdio.h>
#include "../utils/helpers.h"
#include "../utils/image.h"

static double **get_gaussian_filter_kernel(int range, double sigma, int K)
{
    int i, j;
    double sum = 0;
    double **gauss = malloc(sizeof(double *) * range);
    for (int i = 0; i < range; i++)
        gauss[i] = malloc(sizeof(double) * range);

    for (i = 0; i < range; i++)
    {
        for (j = 0; j < range; j++)
        {
            double x = i - (range - 1) / 2.0;
            double y = j - (range - 1) / 2.0;

            gauss[i][j] = K * exp(-(x * x + y * y) / (2 * sigma * sigma));

            sum += gauss[i][j];
        }
    }

    for (i = 0; i < range; i++)
    {
        for (j = 0; j < range; j++)
        {
            gauss[i][j] /= sum;
        }
    }

    return gauss;
}

static void gaussian_blur_pixel(
    Image *image, int x, int y, int range, double **kernel)
{
    Pixel new_pixel;
    new_pixel.r = 0;
    new_pixel.g = 0;
    new_pixel.b = 0;

    int mid = range / 2.0;

    int irange = 0;
    for (int i = -1 * mid; i <= mid; i++, irange++)
    {
        int xx = clamp(x + i, 0, image->width - 1), jrange = 0;

        for (int j = -1 * mid; j <= mid; j++, jrange++)
        {
            int yy = clamp(y + j, 0, image->height - 1);

            double factor = kernel[irange][jrange];
            Pixel *pix = &image->pixels[xx][yy];

            new_pixel.r += factor * pix->r;
            new_pixel.g += factor * pix->g;
            new_pixel.b += factor * pix->b;
        }
    }

    image->pixels[x][y] = new_pixel;
}

void gaussian_blur_image(Image *image, int range, double sigma, int K)
{
    if (range % 2 == 0)
        range++;

    double **kernel = get_gaussian_filter_kernel(range, sigma, K);
    double pnum = image->width * image->height;
    image->average_color = 0;

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            gaussian_blur_pixel(image, x, y, range, kernel);

            image->average_color += image->pixels[x][y].r / pnum;
        }
    }

    for (int i = 0; i < range; i++)
        free(kernel[i]);
    free(kernel);
}

double *get_gaussian_smoothing_kernel(int range, double sigma)
{
    if (range % 2 == 0)
        range++;

    double sum = 0;
    double *gauss = malloc(sizeof(double) * range * range);

    double r, s = 2.0 * sigma * sigma;

    int start = -(range / 2);
    int end = range / 2;

    // generating the Kernel
    int c = 0;
    for (int x = start; x <= end; x++)
    {
        for (int y = start; y <= end; y++)
        {
            r = sqrt(x * x + y * y);
            gauss[c] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += gauss[c];
            c++;
        }
    }

    // normalising the Kernel
    c = 0;
    for (int i = 0; i < range; ++i)
    {
        for (int j = 0; j < range; ++j)
        {
            gauss[c] /= sum;
            c++;
        }
    }

    return gauss;
}