#include <err.h>
#include <math.h>
#include <stdbool.h>
#include "helpers.h"
#include "image.h"
#include "int_list.h"

#define EDGE_COLOR 255
#define LINES_THRESHOLD_PERCENTAGE 30

static void find_edges(int **accumulator, int diag, int rho_num, int theta_num,
    double *rhos, double *thetas, int_list *edges_x, int_list *edges_y,
    Image *out, int w, int h, bool verbose_mode)
{
    if (verbose_mode)
        printf("   📍 Fiding edges...\n");

    // 1. Fiding the maximum
    double max = 0;
    for (int r = 0; r <= rho_num; r++)
    {
        for (int t = 0; t <= theta_num; t++)
        {
            int val = accumulator[r][t];

            if (val == 0)
                continue;

            if (val > max)
                max = val;
        }
    }

    if (verbose_mode)
        printf("   👆 Maximum: %f\n", max);

    // 2. Computing threshold
    int line_threshold = max * (LINES_THRESHOLD_PERCENTAGE / 100.0);

    if (verbose_mode)
        printf("   👈 Threshold: %i\n", line_threshold);

    // 3. Fiding coordinates of the edges in the accumulator using the
    // threshold

    int prev = accumulator[0][0];
    int prev_t = 0, prev_r = 0;
    bool increasing = true;

    int r_c = 255, g_c = 0, b_c = 255;
    int edges = 0;

    for (int r = 0; r <= rho_num; r++)
    {
        for (int t = 0; t <= theta_num; t++)
        {
            int val = accumulator[r][t];

            // Check if the current value is a local maximum. If so, we can
            // treat it, otherwise, we continue the iteration
            if (val >= prev)
            {
                prev = val;
                prev_r = r;
                prev_t = t;
                increasing = true;
                continue;
            }
            else if (val < prev && increasing)
            {
                increasing = false;
            }
            else if (val < prev)
            {
                prev = val;
                prev_r = r;
                prev_t = t;
                continue;
            }

            if (val < line_threshold)
                continue;

            double rho = rhos[prev_r];
            double theta = thetas[prev_t];

            double ax = cos(theta);
            double ay = sin(theta);

            int x0 = rho * ax;
            int y0 = rho * ay;

            int x1 = x0 + diag * (-ay);
            int y1 = y0 + diag * (ax);

            int x2 = x0 - diag * (-ay);
            int y2 = y0 - diag * (ax);

            // 4. Draw edges on output
            int *coordinates
                = draw_line(out, w, h, x1, y1, x2, y2, r_c, g_c, b_c);
            edges++;

            li_append(edges_x, coordinates[0]);
            li_append(edges_y, coordinates[1]);

            li_append(edges_x, coordinates[2]);
            li_append(edges_y, coordinates[3]);

            free(coordinates);

            if (verbose_mode)
                fprintf(stderr, "\33[2K\r   📊 Edges: %i", edges);

            if (g_c == 255)
            {
                r_c++;
                b_c--;
            }
            else if (b_c == 255)
            {
                r_c--;
                g_c++;
            }
            else
            {
                b_c++;
                r_c--;
            }
        }
    }

    if (verbose_mode)
        fprintf(stderr, "\n");
}

int **hough_transform(Image *in, Image *clean, int_list *edges_x,
    int_list *edges_y, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("   🧭 Hough Transform...\n");

    if (verbose_mode)
        printf("   🧮 Computing Edges...\n");

    double w = in->width, h = in->height;
    // int w2 = w / 2, h2 = h / 2;
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

    if (verbose_mode)
        fprintf(stderr, "\33[2K\r   🖨️ Done: %i %%", 0);

    int done = 0;
    float total = w * h;
    // Filling the accumulator
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            done++;
            // Process pixel only if its an edge
            if (in->pixels[x][y].r != EDGE_COLOR)
                continue;

            for (int t = 0; t <= theta_num; t++)
            {
                double rho = x * cos_t[t] + y * sin_t[t];
                int rho_i = rho + diag;

                accumulator[rho_i][t]++;
            }

            int percent = (done / total) * 100;
            if (verbose_mode && percent % 5 == 0)
                fprintf(stderr, "\33[2K\r   🖨️ Done: %i %%", percent);
        }
    }

    if (verbose_mode)
        fprintf(stderr, "\33[2K\r");

    // Converting the accumulator into an image
    Image polar = Array2D_to_Image(accumulator, rho_num + 1, theta_num + 1);
    verbose_save(
        verbose_mode, verbose_path, "7.1-hough-transform-polar.png", &polar);
    free_Image(&polar);

    // Fiding edges from the local maximums of the accumulator
    Image out = clone_image(clean);

    find_edges(accumulator, diag, rho_num, theta_num, rhos, thetas, edges_x,
        edges_y, &out, w, h, verbose_mode);

    verbose_save(
        verbose_mode, verbose_path, "7.2-hough-transform-edges.png", &out);
    free_Image(&out);

    free(rhos);
    free(thetas);
    free(cos_t);
    free(sin_t);

    return accumulator;
}