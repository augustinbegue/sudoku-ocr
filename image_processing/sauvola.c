#include <math.h>
#include "helpers.h"
#include "image.h"

/**
 * @brief Computes the local mean of the image
 *
 * @param input image to compute the mean from
 * @param range size of the window
 * @return 2d array [x * w + y] containing the local means
 */
int *local_mean(Image *in, int range)
{
    int w = in->width;
    int h = in->height;

    if (range % 2 == 0)
        range++;

    double *local_mean_kernel
        = (double *)malloc(range * range * sizeof(double) + 1);
    for (int i = 0; i < range; i++)
        local_mean_kernel[i] = 1 / (double)range;

    int *out = (int *)malloc(w * h * sizeof(int) + 1);
    for (int ix = 0; ix < w; ix++)
        for (int iy = 0; iy < h; iy++)
            out[ix + iy * w] = 0;

    convolution_mat(local_mean_kernel, range, 1, in, out);

    convolution_mat(local_mean_kernel, 1, range, in, out);

    free(local_mean_kernel);

    return out;
}

/**
 * @brief Computes the standard deviation of an image
 *
 * @param in input image
 * @param range size of the window
 * @param local_mean_in local means of the image
 * @return 2d array [x * w + y] containing the standard deviation
 */
int *standard_deviation(Image *in, int range, int *local_mean_in)
{
    int w = in->width;
    int h = in->height;

    int *out = (int *)malloc(sizeof(int) * w * h + 1);
    for (int ix = 0; ix < w; ix++)
        for (int iy = 0; iy < h; iy++)
            out[ix + iy * w] = 0;

    if (range % 2 == 0)
        range++;
    int rangehalf = range / 2;

    for (int ix = 0; ix < w; ix++)
    {
        for (int iy = 0; iy < h; iy++)
        {
            double convoluted_value = 0;
            double mean = local_mean_in[ix + iy * w];

            for (int i = -rangehalf; i <= rangehalf; i++)
            {
                for (int j = -rangehalf; j <= rangehalf; j++)
                {
                    int x = ix - i;
                    int y = iy - j;

                    if (0 > x)
                        x = 0;
                    if (x >= w)
                        x = w - 1;
                    if (0 > y)
                        y = 0;
                    if (y >= h)
                        y = h - 1;

                    double color = in->pixels[x][y].r;
                    convoluted_value += pow(color - mean, 2);
                }
            }

            out[ix + iy * w] = convoluted_value / pow(range, (double)2);
        }
    }

    return out;
}

/**
 * @brief Applies the Sauvola thresholding method onto an image
 *
 * @param in input image
 * @param out output image
 * @param range width of the filter's window
 * @param k arbitrary coefficient ∊ [0.2, 0.5]
 */
void filter_sauvola(Image *in, Image *out, int range, double k,
    bool verbose_mode, char *verbose_path)
{
    int w = in->width;
    int h = in->height;

    if (range % 2 == 0)
        range++;

    int *local_mean_in = local_mean(in, range);

    Image local_mean_img = clone_image(in);
    Array_to_Image(local_mean_in, &local_mean_img);
    verbose_save(
        verbose_mode, verbose_path, "2.1-local_mean.png", &local_mean_img);
    free_Image(&local_mean_img);

    int *standard_deviation_in = standard_deviation(in, range, local_mean_in);

    Image standard_deviation_img = clone_image(in);
    Array_to_Image(standard_deviation_in, &standard_deviation_img);
    verbose_save(verbose_mode, verbose_path, "2.2-standard_deviation.png",
        &standard_deviation_img);
    free_Image(&standard_deviation_img);

    double R = 128;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int threshold
                = local_mean_in[x + y * w]
                  * (1 + k * (standard_deviation_in[x + y * w] / R - 1));

            int color = out->pixels[x][y].r > threshold ? 0 : 255;

            out->pixels[x][y].r = out->pixels[x][y].g = out->pixels[x][y].b
                = color;
        }
    }

    free(local_mean_in);
    free(standard_deviation_in);
}