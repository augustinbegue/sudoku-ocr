#include <err.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "image.h"

/**
 * @brief Convolution operator
 *
 * @link https://en.wikipedia.org/wiki/Kernel_(image_processing)#Convolution
 *
 * @param kernel kernel of the convolution
 * @param ksize size of the kernel
 * @param image image to convolute from
 * @param out output
 * @param normalize normalize values
 */
void convolution(
    double *kernel, int ksize, Image *image, Image *out, bool normalize)
{
    const int khalf = ksize / 2;

    int w = image->width;
    int h = image->height;

    float min = FLT_MAX, max = -FLT_MAX;

    float *pixels = malloc(sizeof(float) * w * h);
    int pindex = 0;

    for (int ix = 0; ix < w; ix++)
    {
        for (int iy = 0; iy < h; iy++)
        {
            float pixel = 0.0;
            size_t c = 0;
            for (int j = -khalf; j <= khalf; j++)
            {
                for (int i = -khalf; i <= khalf; i++)
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

                    pixel += image->pixels[x][y].r * kernel[c];

                    c++;
                }
            }

            if (normalize && pixel < min)
                min = pixel;

            if (normalize && pixel > max)
                max = pixel;

            pixels[pindex] = pixel;
            pindex++;
        }
    }

    pindex = 0;
    for (int m = 0; m < w; m++)
    {
        for (int n = 0; n < h; n++)
        {
            float pixel = pixels[pindex];

            if (normalize)
                pixel = 255 * (pixel - min) / (max - min);

            Pixel pix = {pixel, pixel, pixel};
            out->pixels[m][n] = pix;
            pindex++;
        }
    }

    free(pixels);
}

/**
 * @brief Convolution operator. Returns a 2d array.
 *
 * @param kernel
 * @param ksize_x
 * @param ksize_y
 * @param in
 * @param out
 */
void convolution_mat(
    double *kernel, int ksize_x, int ksize_y, Image *in, double *out)
{
    int w = in->width;
    int h = in->height;

    if (ksize_x % 2 == 0)
        ksize_x--;

    if (ksize_y % 2 == 0)
        ksize_y--;

    int khalf_x = ksize_x / 2;
    int khalf_y = ksize_y / 2;

    for (int ix = 0; ix < w; ix++)
    {
        for (int iy = 0; iy < h; iy++)
        {
            double convoluted_value = 0;
            int c = 0;

            for (int i = -khalf_x; i <= khalf_x; i++)
            {
                for (int j = -khalf_y; j <= khalf_y; j++)
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

                    convoluted_value += in->pixels[x][y].r * kernel[c];

                    c++;
                }
            }

            out[ix * w + iy] = convoluted_value;
        }
    }
}

void _mkdir(const char *dir)
{
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/')
        {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}

double clamp(double d, double min, double max)
{
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

void verbose_save(
    bool verbose_mode, char *verbose_path, char *file_name, Image *image)
{
    if (verbose_mode)
    {
        char t[strlen(verbose_path) + strlen(file_name) + 2];
        strcpy(t, verbose_path);
        strcat(t, "/");
        strcat(t, file_name);
        save_image(Image_to_SDL_Surface(image), t);
        return;
    }
    return;
}

double degrees_to_rad(double degrees)
{
    return degrees * (3.14159265359 / 180);
}

double *spread_arr(int size, double min, double max, double step)
{
    double *array = malloc(sizeof(double) * size + 1);

    double current = min;
    for (int i = 0; i < size && current <= max; i++)
    {
        array[i] = current;
        current += step;
    }

    return array;
}

int *substract_abs_arr(int in[], int size, int value)
{
    int *array = malloc(sizeof(int) * size + 1);

    if (array == NULL)
    {
        errx(1, "substract_abs_arr: error when allocating memory");
    }

    for (int i = 0; i < size; i++)
    {
        array[i] = abs(in[i] - value);
    }

    return array;
}

int min_arr_index(int array[], int size)
{
    int min = array[0];

    for (int i = 0; i < size; i++)
    {
        int val = array[i];

        if (val < min)
        {
            min = val;
        }
    }

    return min;
}

void free_2d_arr(int **arr, int size)
{
    for (int i = 0; i < size; i++)
        free(arr[i]);
    free(arr);
}