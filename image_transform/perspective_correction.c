#include <math.h>
#include <stdbool.h>
#include "float.h"
#include "geometry.h"
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

/**
 * @brief Cross product of two vectors array
 *
 * @param vect_A
 * @param vect_B
 * @param cross_P
 */
void cross_product(double vect_A[], double vect_B[], double cross_P[])
{

    cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
    cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
}

Image correct_perspective(Image *image, square *selected_square,
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
    int x_displacement = destination[0][0] - source[0][0];
    int y_displacement = destination[0][1] - source[0][1];
    double translation_matrix[3][3]
        = {{1, 0, x_displacement}, {0, 1, y_displacement}, {0, 0, 1}};
    if (verbose_mode)
        printf("    Translation factors - x: %d, y: %d\n", x_displacement,
            y_displacement);

    // Shear Matrix
    // Computing shear factors :
    // Difference between the coordinates of the edges that are supposed to be
    // parallel
    double x_shear_factor = (source[0][0] - source[3][0]) / max_edge_length;
    double y_shear_factor = (source[0][1] - source[1][1]) / max_edge_length;
    double shear_matrix[3][3]
        = {{1, y_shear_factor, 0}, {x_shear_factor, 1, 0}, {0, 0, 1}};
    if (verbose_mode)
        printf("    Shear factors - x: %f, y: %f\n", x_shear_factor,
            y_shear_factor);

    // Perspective Transform
    // Computing perspective factors :
    // We select two sets of lines that are supposed to be parallel.
    // In our case, theses are the edges of the square
    // We compute the intersection of these lines
    line horizontal_1
        = {source[0][0], source[0][1], source[1][0], source[1][1]};
    line horizontal_2
        = {source[2][0], source[2][1], source[3][0], source[3][1]};

    // Line 1 represented as a1 + b1 = c1
    double h1_eq[3] = {horizontal_1.y1 - horizontal_1.y0,
        horizontal_1.x0 - horizontal_1.x1, 1};
    h1_eq[2] = h1_eq[0] * horizontal_1.x0 + h1_eq[1] * horizontal_1.y0;

    // Line 2 represented as a2 + b2 = c2
    double h2_eq[3] = {horizontal_2.y1 - horizontal_2.y0,
        horizontal_2.x0 - horizontal_2.x1, 1};
    h2_eq[2] = h2_eq[0] * horizontal_2.x0 + h2_eq[1] * horizontal_2.y0;

    double h_intersection[3] = {0, 0, 0};

    cross_product(h1_eq, h2_eq, h_intersection);

    line vertical_1 = {source[0][0], source[0][1], source[3][0], source[3][1]};
    line vertical_2 = {source[1][0], source[1][1], source[2][0], source[2][1]};

    // Line 1 represented as a1 + b1 = c1
    double v1_eq[3]
        = {vertical_1.y1 - vertical_1.y0, vertical_1.x0 - vertical_1.x1, 1};
    v1_eq[2] = v1_eq[0] * vertical_1.x0 + v1_eq[1] * vertical_1.y0;

    // Line 2 represented as a2 + b2 = c2
    double v2_eq[3]
        = {vertical_2.y1 - vertical_2.y0, vertical_2.x0 - vertical_2.x1, 1};
    v2_eq[2] = v2_eq[0] * vertical_2.x0 + v2_eq[1] * vertical_2.y0;

    double v_intersection[3] = {0, 0, 0};

    cross_product(v1_eq, v2_eq, v_intersection);

    if (verbose_mode)
        printf("    Horizontal Intersection -  (%.2f, %.2f, %.2f)\n",
            h_intersection[0], h_intersection[1], h_intersection[2]);

    if (verbose_mode)
        printf("    Vertical Intersection - (%.2f, %.2f, %.2f)\n",
            v_intersection[0], v_intersection[1], v_intersection[2]);

    double perspective_factor_1 = 0;
    double perspective_factor_2 = 0;

    // Then, if there is an intersection, we do the cross product of the two
    // intersection points to obtain the line at infinity
    if (h_intersection[2] != 0 && v_intersection[2] != 0
        && (h_intersection[0] >= max_edge_length
            || h_intersection[1] >= max_edge_length)
        && (v_intersection[0] >= max_edge_length
            || v_intersection[1] >= max_edge_length))
    {

        double projection[3] = {0, 0, 0};
        cross_product(v_intersection, h_intersection, projection);

        if (verbose_mode)
            printf("    Projection - (%.2f, %.2f, %.2f)\n", v_intersection[0],
                v_intersection[1], v_intersection[2]);

        perspective_factor_1 = projection[0] / projection[2];
        perspective_factor_2 = projection[1] / projection[2];
    }

    if (verbose_mode)
        printf("    Perspective factors - 1: %f, 2: %f\n",
            perspective_factor_1, perspective_factor_2);

    double perspective_matrix[3][3] = {
        {1, 0, 0}, {0, 1, 0}, {perspective_factor_1, perspective_factor_2, 1}};

    // Final Transformation Matrix
    double transformation_matrix[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    double transformation_matrix_inv[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

    multiply_matrix(transformation_matrix, translation_matrix);
    multiply_matrix(transformation_matrix, shear_matrix);
    multiply_matrix(transformation_matrix, perspective_matrix);

    inverse_matrix(transformation_matrix, transformation_matrix_inv);

    Image corrected_image = create_image(max_edge_length, max_edge_length);

    // Applying the transformation matrix to the image
    for (int i = 0; i < corrected_image.height; i++)
    {
        for (int j = 0; j < corrected_image.width; j++)
        {
            double ut = i;
            double vt = j;
            double wt = transformation_matrix[2][0] * ut
                        + transformation_matrix[2][1] * vt
                        + transformation_matrix[2][2];

            double old_coordinates[3] = {ut / wt, vt / wt, 1};

            int x = transformation_matrix_inv[0][0] * old_coordinates[0]
                    + transformation_matrix_inv[0][1] * old_coordinates[1]
                    + transformation_matrix_inv[0][2] * old_coordinates[2];

            int y = transformation_matrix_inv[1][0] * old_coordinates[0]
                    + transformation_matrix_inv[1][1] * old_coordinates[1]
                    + transformation_matrix_inv[1][2] * old_coordinates[2];

            if (x >= 0 && y >= 0 && x < image->width && y < image->height)
            {
                corrected_image.pixels[i][j] = image->pixels[x][y];
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
