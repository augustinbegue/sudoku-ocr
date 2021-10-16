#include <err.h>
#include <math.h>
#include <stdbool.h>
#include "image.h"

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
    }
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