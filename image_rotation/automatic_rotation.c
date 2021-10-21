#include "geometry.h"
#include "helpers.h"
#include "image.h"
#include "rotation.h"

Image automatic_rotation(int **hough_accumulator, square *found_grid_square,
    Image *in, bool verbose_mode, char *verbose_path)
{
    double w = in->width, h = in->height;
    double diag = sqrt(w * w + h * h);
    int size = diag * 2;

    double theta_min = -90, theta_max = 90,
           theta_step = (theta_max - theta_min) / size;

    double *thetas = spread_arr(size + 1, theta_min, theta_max, theta_step);

    int *thetas_votes = malloc(sizeof(int) * (size + 1) + 1);

    for (int r = 0; r <= size; r++)
    {
        thetas_votes[r] = 0;
    }

    for (int r = 0; r <= size; r++)
    {
        for (int t = 0; t <= size; t++)
        {
            if (hough_accumulator[r][t] >= 255)
            {
                thetas_votes[t] += hough_accumulator[r][t];
            }
        }

        free(hough_accumulator[r]);
    }

    double most_freq_theta = 0;
    double most_freq_votes = thetas_votes[0];

    for (int i = 1; i <= size; i++)
    {
        int votes = thetas_votes[i];

        if (votes > most_freq_votes)
        {
            most_freq_theta = thetas[i];
            most_freq_votes = votes;
        }
    }

    free(hough_accumulator);
    free(thetas);
    free(thetas_votes);

    if (most_freq_theta < 0)
        most_freq_theta = most_freq_theta + 90;

    if (verbose_mode)
        printf("   🎣 Most found angle for automatic rotation: %.2f\n",
            most_freq_theta);

    if (most_freq_theta >= 1 && most_freq_theta <= 45)
    {
        double x = 0, y = 0;

        double center_x = (w / (double)2);
        double center_y = (h / (double)2);

        double theta_rad = -degrees_to_rad(most_freq_theta);

        corrected_rotation_matrix(theta_rad, found_grid_square->c1.x,
            found_grid_square->c1.y, center_x, center_y, &x, &y);
        found_grid_square->c1.x = x;
        found_grid_square->c1.y = y;

        corrected_rotation_matrix(theta_rad, found_grid_square->c2.x,
            found_grid_square->c2.y, center_x, center_y, &x, &y);
        found_grid_square->c2.x = x;
        found_grid_square->c2.y = y;

        corrected_rotation_matrix(theta_rad, found_grid_square->c3.x,
            found_grid_square->c3.y, center_x, center_y, &x, &y);
        found_grid_square->c3.x = x;
        found_grid_square->c3.y = y;

        corrected_rotation_matrix(theta_rad, found_grid_square->c4.x,
            found_grid_square->c4.y, center_x, center_y, &x, &y);
        found_grid_square->c4.x = x;
        found_grid_square->c4.y = y;

        return rotate_image(in, most_freq_theta);
    }

    return clone_image(in);
}