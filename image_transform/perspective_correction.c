#include <math.h>
#include <stdbool.h>
#include "float.h"
#include "geometry.h"
#include "helpers.h"
#include "image.h"
#include "inverse_matrix.h"

#define MAX 9
#define N 9

void free_mat(double **mat, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(mat[i]);
    }
    free(mat);
}

/**
 * @brief In place multiplication of transform matrix A by B
 */
void multiply_mat(double **A, double **B, int size)
{
    double **C = calloc(size, sizeof(double *));
    for (int i = 0; i < size; i++)
        C[i] = calloc(size, sizeof(double));

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                C[i][j] += A[i][k] * B[k][j];

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            A[i][j] = C[i][j];

    for (int i = 0; i < size; i++)
        free(C[i]);
    free(C);
}

/**
 * @brief Inverts a 3x3 matrix
 */
void inverse_3x3_matrix(double **M, double **M_inv)
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
 * @brief Multiply a size*size matrix by a column vector
 */
void multiply_mat_vector(
    double M[][MAX], double v[MAX], double v_out[MAX], int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            v_out[i] += M[i][j] * v[j];
}

void multiply_mat_vector_pt(double **M, double *v, double *v_out, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            v_out[i] += M[i][j] * v[j];
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

void compute_transformation_matrix(int source[][2], double destination[][2],
    int max_edge_length, double **transformation_matrix,
    double **transformation_matrix_inv, bool verbose_mode, char *verbose_path)
{
    /*
     * Computing affine transformation matrixes
     */

    // Translation matrix
    int x_displacement = destination[0][0] - source[0][0];
    int y_displacement = destination[0][1] - source[0][1];
    double **translation_matrix = calloc(3, sizeof(double *));
    for (int i = 0; i < 3; i++)
        translation_matrix[i] = calloc(3, sizeof(double));

    transformation_matrix[0][0] = 1;
    transformation_matrix[0][2] = x_displacement;
    transformation_matrix[1][1] = 1;
    transformation_matrix[1][2] = y_displacement;
    transformation_matrix[2][2] = 1;

    if (verbose_mode)
        printf("    Translation factors - x: %d, y: %d\n", x_displacement,
            y_displacement);

    // Shear Matrix
    // Computing shear factors :
    // Difference between the coordinates of the edges that are supposed to be
    // parallel
    double x_shear_factor = (source[0][0] - source[3][0]) / max_edge_length;
    double y_shear_factor = (source[0][1] - source[1][1]) / max_edge_length;
    double **shear_matrix = calloc(3, sizeof(double *));
    for (int i = 0; i < 3; i++)
        shear_matrix[i] = calloc(3, sizeof(double));

    transformation_matrix[0][0] = 1;
    transformation_matrix[0][1] = y_shear_factor;
    transformation_matrix[1][1] = 1;
    transformation_matrix[1][0] = x_shear_factor;
    transformation_matrix[2][2] = 1;

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

    double **perspective_matrix = calloc(3, sizeof(double *));
    for (int i = 0; i < 3; i++)
        perspective_matrix[i] = calloc(3, sizeof(double));

    perspective_matrix[0][0] = 1;
    perspective_matrix[1][1] = 1;
    perspective_matrix[2][0] = perspective_factor_1;
    perspective_matrix[2][1] = perspective_factor_2;
    perspective_matrix[2][2] = 1;

    // Final Transformation Matrix
    transformation_matrix[0][0] = 1;
    transformation_matrix[1][1] = 1;
    transformation_matrix[2][2] = 1;

    multiply_mat(transformation_matrix, translation_matrix, 3);
    multiply_mat(transformation_matrix, shear_matrix, 3);
    multiply_mat(transformation_matrix, perspective_matrix, 3);

    inverse_3x3_matrix(transformation_matrix, transformation_matrix_inv);

    free_mat(translation_matrix, 3);
    free_mat(shear_matrix, 3);
    free_mat(perspective_matrix, 3);
}

void compute_perspective_matrix(int source[][2], double dest[][2],
    double **transformation_matrix, double **transformation_matrix_inv)
{
    double P[][9] = {
        {-source[0][0], -source[0][1], -1, 0, 0, 0, source[0][0] * dest[0][0],
            source[0][1] * dest[0][0], dest[0][0]},
        {0, 0, 0, -source[0][0], -source[0][1], -1, source[0][0] * dest[0][1],
            source[0][1] * dest[0][1], dest[0][1]},
        {-source[1][0], -source[1][1], -1, 0, 0, 0, source[1][0] * dest[1][0],
            source[1][1] * dest[1][0], dest[1][0]},
        {0, 0, 0, -source[1][0], -source[1][1], -1, source[1][0] * dest[1][1],
            source[1][1] * dest[1][1], dest[1][1]},
        {-source[2][0], -source[2][1], -1, 0, 0, 0, source[2][0] * dest[2][0],
            source[2][1] * dest[2][0], dest[2][0]},
        {0, 0, 0, -source[2][0], -source[2][1], -1, source[2][0] * dest[2][1],
            source[2][1] * dest[2][1], dest[2][1]},
        {-source[3][0], -source[3][1], -1, 0, 0, 0, source[3][0] * dest[3][0],
            source[3][1] * dest[3][0], dest[3][0]},
        {0, 0, 0, -source[3][0], -source[3][1], -1, source[3][0] * dest[3][1],
            source[3][1] * dest[3][1], dest[3][1]},
        {0, 0, 0, 0, 0, 0, 0, 0, 1}};

    // Print P
    if (true)
    {
        printf("\nP:\n");
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
                printf("%.2f ", P[i][j]);
            printf("\n");
        }
    }

    double R[9] = {0, 0, 0, 0, 0, 0, 0, 0, 1};

    double P_inv[9][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}};

    // Invert matrix P
    inverse_mat(P, P_inv, 9);

    // Print P^-1
    if (true)
    {
        printf("\nP^-1:\n");
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
                printf("%.2f ", P_inv[i][j]);
            printf("\n");
        }
    }

    // Compute H = P_inv * R
    double *H = calloc(9, sizeof(double));
    multiply_mat_vector(P_inv, R, H, 9);

    // Print H
    if (true)
    {
        printf("\nH:\n");
        for (int i = 0; i < 9; i++)
            printf("%.2f ", H[i]);
        printf("\n");
    }

    // Convert H to 3x3 matrix
    int v = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++, v++)
        {
            transformation_matrix[i][j] = H[v];
        }
    }

    inverse_3x3_matrix(transformation_matrix, transformation_matrix_inv);

    free(H);
}

Image *correct_perspective(Image *image, square *selected_square,
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

    double destination[4][2] = {{0, 0}, {max_edge_length, 0},
        {max_edge_length, max_edge_length}, {0, max_edge_length}};

    double **transformation_matrix = calloc(3, sizeof(double *));
    for (int i = 0; i < 3; i++)
        transformation_matrix[i] = calloc(3, sizeof(double));

    double **transformation_matrix_inv = calloc(3, sizeof(double *));
    for (int i = 0; i < 3; i++)
        transformation_matrix_inv[i] = calloc(3, sizeof(double));

    compute_perspective_matrix(
        source, destination, transformation_matrix, transformation_matrix_inv);

    // Testing the transformation matrix

    if (verbose_mode)
    {
        printf("Testing the transformation matrix...\n");

        // Printing the matrix
        printf("Transformation matrix:\n");
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
                printf("%f ", transformation_matrix[i][j]);
            printf("\n");
        }

        for (int i = 0; i < 4; i++)
        {
            double v1[3] = {source[i][0], source[i][1], 1};
            double res1[3] = {0, 0, 0};

            multiply_mat_vector_pt(transformation_matrix, v1, res1, 3);

            double x1 = res1[0] / res1[2];
            double y1 = res1[1] / res1[2];

            printf("   [%i] EXPECTED: (%f,%f), GOT: (%f,%f)\n", i + 1,
                destination[i][0], destination[i][1], x1, y1);
        }
    }

    Image *corrected_image = create_image(max_edge_length, max_edge_length);

    for (int i = 0; i < corrected_image->height; i++)
    {
        for (int j = 0; j < corrected_image->width; j++)
        {
            double ut = i;
            double vt = j;
            double wt = 1;

            double old_coordinates[3] = {ut, vt, wt};
            double new_coordinates[3] = {0, 0, 0};

            multiply_mat_vector_pt(transformation_matrix_inv, old_coordinates,
                new_coordinates, 3);

            int x = (int)(new_coordinates[0] / new_coordinates[2]);
            int y = (int)(new_coordinates[1] / new_coordinates[2]);

            if (x >= 0 && y >= 0 && x < image->width && y < image->height)
            {
                corrected_image->pixels[i][j] = image->pixels[x][y];
            }
            else
            {
                Pixel pix = {.r = 0, .g = 0, .b = 0};
                corrected_image->pixels[i][j] = pix;
            }
        }
    }

    verbose_save(verbose_mode, verbose_path, "9.1-perspective-corrected.png",
        corrected_image);

    free_mat(transformation_matrix, 3);
    free_mat(transformation_matrix_inv, 3);

    return corrected_image;
}