#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "image.h"

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