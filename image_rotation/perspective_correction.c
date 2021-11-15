#include <math.h>
#include <stdbool.h>
#include "helpers.h"
#include "image.h"

/**
 * @brief In place multiplication of transform matrix A by B
 */
void multiply_matrix(double A[][3], double B[][3])
{
    double C[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                C[i][j] += A[i][k] * B[k][j];

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            A[i][j] = C[i][j];
}

/**
 * @brief Inverts a 3x3 matrix
 */
void inverse_matrix(double M[][3], double M_inv[][3])
{
    double MM = M[0][0] * M[1][1] * M[2][2] + M[0][1] * M[1][2] * M[2][0]
                + M[0][2] * M[2][1] * M[1][0] - M[0][2] * M[1][1] * M[2][0]
                - M[0][1] * M[1][0] * M[2][2] - M[0][0] * M[2][1] * M[1][2];

    double AM[3][3] = {{M[1][1] * M[2][2] - M[1][2] * M[2][1],
                           M[0][2] * M[2][1] - M[0][1] * M[2][2],
                           M[0][1] * M[1][2] - M[0][2] * M[1][1]},
        {M[1][2] * M[2][0] - M[1][0] * M[2][2],
            M[0][0] * M[2][2] - M[0][2] * M[2][0],
            M[0][2] * M[1][0] - M[0][0] * M[1][2]},
        {M[1][0] * M[2][1] - M[1][1] * M[2][0],
            M[0][1] * M[2][0] - M[0][0] * M[2][1],
            M[0][0] * M[1][1] - M[0][1] * M[1][0]}};

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            M_inv[i][j] = AM[i][j] / MM;
}

/**
 * @brief Multiply a  3x3 matrix by a column vector
 */
void multiply_matrix_vector(double M[][3], double v[3], double v_out[3])
{
    for (int i = 0; i < 3; i++)
        v_out[i] = M[i][0] * v[0] + M[i][1] * v[1] + M[i][2] * v[2];
}

Image correct_perspective(Image *rotated_imagept, square *selected_square,
    bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("   🗺️ Correcting perspective and cropping...\n");

    int source[4][2] = {{selected_square->c1.x, selected_square->c1.y},
        {selected_square->c2.x, selected_square->c2.y},
        {selected_square->c3.x, selected_square->c3.y},
        {selected_square->c4.x, selected_square->c4.y}};

    int edge_1_length = sqrt(pow(source[0][0] - source[1][0], 2)
                             + pow(source[0][1] - source[1][1], 2));
    int edge_2_length = sqrt(pow(source[1][0] - source[2][0], 2)
                             + pow(source[1][1] - source[2][1], 2));
    int edge_3_length = sqrt(pow(source[2][0] - source[3][0], 2)
                             + pow(source[2][1] - source[3][1], 2));
    int edge_4_length = sqrt(pow(source[3][0] - source[0][0], 2)
                             + pow(source[3][1] - source[0][1], 2));

    double max_edge_length = fmax(fmax(edge_1_length, edge_2_length),
        fmax(edge_3_length, edge_4_length));

    double min_edge_length = fmin(fmin(edge_1_length, edge_2_length),
        fmin(edge_3_length, edge_4_length));

    double destination[4][2] = {{0, 0}, {max_edge_length, 0},
        {max_edge_length, max_edge_length}, {0, max_edge_length}};

    /*
     * Computing affine transformation matrixes
     */

    // Translation matrix
    double x_displacement = destination[0][0] - source[0][0];
    double y_displacement = destination[0][1] - source[0][1];
    double translation_matrix[3][3]
        = {{1, 0, 0}, {0, 1, 0}, {x_displacement, y_displacement, 1}};
    if (verbose_mode)
        printf("    Translation factors - x: %f, y: %f\n", x_displacement,
            y_displacement);

    // Scale matrix
    // Computing scale factors :
    // Average between the factors for each edge
    double x_scale_factor = 1;
    double y_scale_factor = 1;
    double scale_matrix[3][3]
        = {{x_scale_factor, 0, 0}, {0, y_scale_factor, 0}, {0, 0, 1}};
    if (verbose_mode)
        printf("    Scale factors - x: %f, y: %f\n", x_scale_factor,
            y_scale_factor);

    // Shear Matrix
    // Computing shear factors :
    // Difference between the coordinates of the edges that are supposed to be
    // parallel
    double x_shear_factor = 0;
    //= (source[0][0] - source[3][0] + source[1][0] - source[2][0]) / 2;
    double y_shear_factor = 0;
    //= (source[0][1] - source[1][1] + source[2][1] - source[3][1]) / 2;
    double shear_matrix[3][3]
        = {{1, x_shear_factor, 0}, {y_shear_factor, 1, 0}, {0, 0, 1}};
    if (verbose_mode)
        printf("    Shear factors - x: %f, y: %f\n", x_shear_factor,
            y_shear_factor);

    // Final Transformation Matrix
    double transformation_matrix[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    double transformation_matrix_inv[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    multiply_matrix(transformation_matrix, translation_matrix);
    multiply_matrix(transformation_matrix, scale_matrix);
    multiply_matrix(transformation_matrix, shear_matrix);

    inverse_matrix(transformation_matrix, transformation_matrix_inv);

    Image corrected_image = create_image(max_edge_length, max_edge_length);

    // Applying the transformation matrix to the image
    for (int i = 0; i < corrected_image.height; i++)
    {
        for (int j = 0; j < corrected_image.width; j++)
        {
            double x = i;
            double y = j;
            double z = 1;

            double old_coordinates[3] = {x, y, z};
            double new_coordinates[3] = {0, 0, 0};
            multiply_matrix_vector(
                transformation_matrix_inv, old_coordinates, new_coordinates);

            new_coordinates[0] = new_coordinates[0] / new_coordinates[2];
            new_coordinates[1] = new_coordinates[1] / new_coordinates[2];
            new_coordinates[2] = 1;

            if (new_coordinates[0] >= 0 && new_coordinates[1] >= 0
                && new_coordinates[0] < rotated_imagept->width
                && new_coordinates[1] < rotated_imagept->height)
            {
                corrected_image.pixels[i][j] = rotated_imagept->pixels[(
                    int)new_coordinates[0]][(int)new_coordinates[1]];
            }
            else
            {
                Pixel pix = {.r = 0, .g = 0, .b = 0};
                corrected_image.pixels[i][j] = pix;
            }
        }
    }

    verbose_save(verbose_mode, verbose_path, "9-perspective-corrected.png",
        &corrected_image);

    return corrected_image;
}
