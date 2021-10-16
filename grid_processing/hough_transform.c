#include <err.h>
#include <math.h>
#include <stdbool.h>
#include "../utils/helpers.h"
#include "../utils/image.h"

#define EDGE_COLOR 255

Image hough_transform(Image *in, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("   🧭 Hough Transform...\n");

    double w = in->width, h = in->height;
    double diag = sqrt(w * w + h * h);

    // Creating the rho values array
    double rho_min = -diag, rho_max = diag, rho_num = (diag * 2),
           rho_step = (rho_max - rho_min) / rho_num;
    double *rhos = spread_arr(rho_num + 1, rho_min, rho_max, rho_step);

    // Creating the theta array
    double theta_min = -90, theta_max = 90, theta_num = rho_num,
           theta_step = (theta_max - theta_min) / theta_num;
    double *thetas
        = spread_arr(theta_num + 1, theta_min, theta_max, theta_step);

    // Creating caching arrays for faster computation
    double *cos_t = malloc(sizeof(double) * (theta_num + 1));
    double *sin_t = malloc(sizeof(double) * (theta_num + 1));
    for (int i = 0; i <= theta_num; i++)
    {
        thetas[i] = degrees_to_rad(thetas[i]);
        cos_t[i] = cos(thetas[i]);
        sin_t[i] = sin(thetas[i]);
    }

    // Creating accumulator
    int **accumulator = malloc(sizeof(int *) * (rho_num + 1) + 1);
    for (int r = 0; r <= rho_num; r++)
    {
        accumulator[r] = malloc(sizeof(int) * (theta_num + 1) + 1);
        for (int t = 0; t <= theta_num; t++)
            accumulator[r][t] = 0;
    }

    // Filling the accumulator
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            if (in->pixels[x][y].r != EDGE_COLOR)
                continue;

            for (int t = 0; t <= theta_num; t++)
            {
                double rho = x * cos_t[t] + y * sin_t[t];
                int rho_i = rho + diag;

                accumulator[rho_i][t]++;
            }
        }
    }

    // Converting the accumulator into an image
    Image out = Array2D_to_Image(accumulator, rho_num + 1, theta_num + 1);
    verbose_save(verbose_mode, verbose_path, "7-hough-transform.png", &out);

    free(rhos);
    free(thetas);
    free(cos_t);
    free(sin_t);
    for (int r = 0; r <= rho_num; r++)
        free(accumulator[r]);
    free(accumulator);

    return out;
}