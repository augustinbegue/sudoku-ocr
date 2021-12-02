#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include "image.h"

#define M_PI 3.14159265359

void insertionSort(int arr[], int n);

void _mkdir(const char *dir);

double clamp(double d, double min, double max);

void verbose_save(
    bool verbose_mode, char *verbose_path, char *file_name, Image *image);

double degrees_to_rad(double degrees);

double *spread_arr(int size, double min, double max, double step);

int *substract_abs_arr(int in[], int size, int value);

int min_arr_index(int array[], int size);

void free_2d_arr(int **arr, int size);

void convolution(
    double *kernel, int ksize, Image *image, Image *out, bool normalize);

void convolution_mat(
    double *kernel, int ksize_x, int ksize_y, Image *in, int *out);

#endif