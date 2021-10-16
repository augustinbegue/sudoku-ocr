#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include "./image.h"

#define M_PI 3.14159265359

double clamp(double d, double min, double max);

void verbose_save(
    bool verbose_mode, char *verbose_path, char *file_name, Image *image);

double degrees_to_rad(double degrees);

double *spread_arr(int size, double min, double max, double step);

int *substract_abs_arr(int in[], int size, int value);

int min_arr_index(int array[], int size);

#endif