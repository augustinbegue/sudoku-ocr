#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../network.h"
#include "image.h"
#include "maths_extra.h"
#include "matrix.h"

/*
 return the result of the network for a XOR (have to be modified for the real
 network) hw -> hidden weights , hb -> hidden bias , ow -> output weights , ob
 -> ourpur bias
*/
int result_network(
    Image *image, Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob)
{
    // matrix with the inputs
    Matrix input;
    m_init(&input, 1, __num_inputs);

    int count = 0;
    int width = image->width;
    int height = image->height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Pixel pix = image->pixels[x][y];

            // color 0 or 255
            int color = pix.r;
            if (color == 0)
            {
                m_setIndex(&input, 0, count, 0.0);
            }
            else
            {
                m_setIndex(&input, 0, count, 1.0);
            }
            count++;
        }
    }

    // result of the hidden layer
    Matrix result_hidden;
    m_mult(&input, hw, &result_hidden);
    m_add(&result_hidden, hb);
    m_map(&result_hidden, relu);

    // output layer
    Matrix result_output;
    m_mult(&result_hidden, ow, &result_output);
    m_add(&result_output, ob);
    softmax(&result_output);

    int res = max_mat(&result_output);

    // m_full_print(&input);
    // m_full_print(&result_hidden);
    m_full_print(&result_output);

    // free all memory
    m_free(&input);
    m_free(&result_hidden);
    m_free(&result_output);

    return res;
}

/**
 * @brief Executes the neural network on the given image
 *
 * @param image image to test
 * @return int
 */
int neural_network_execute(Image *image)
{
    int retur = 0;
    char *filename = "save";
    FILE *fp;
    fp = fopen(filename, "r");

    if (fp == NULL)
        return 1;

    int num_inputs;
    int num_hidden;
    int num_outputs;

    retur = fscanf(fp, "%d", &num_inputs);
    retur = fscanf(fp, "%d", &num_hidden);

    Matrix hidden_weights;
    m_init(&hidden_weights, num_inputs, num_hidden);

    Matrix *hw = &hidden_weights;
    double tmp = 0.00;

    for (int i = 0; i < hw->rows; i++)
    {
        for (int j = 0; j < hw->cols; j++)
        {
            retur = fscanf(fp, "%lf", &tmp);
            m_setIndex(hw, i, j, tmp);
        }
    }

    retur = fscanf(fp, "%d", &num_inputs);
    retur = fscanf(fp, "%d", &num_hidden);

    Matrix hidden_bias;
    m_init(&hidden_bias, 1, num_hidden);

    Matrix *hb = &hidden_bias;

    for (int i = 0; i < hb->rows; i++)
    {
        for (int j = 0; j < hb->cols; j++)
        {
            retur = fscanf(fp, "%lf", &tmp);
            m_setIndex(hb, i, j, tmp);
        }
    }

    retur = fscanf(fp, "%d", &num_hidden);
    retur = fscanf(fp, "%d", &num_outputs);

    Matrix output_weights;
    m_init(&output_weights, num_hidden, num_outputs);

    Matrix *ow = &output_weights;

    for (int i = 0; i < ow->rows; i++)
    {
        for (int j = 0; j < ow->cols; j++)
        {
            retur = fscanf(fp, "%lf", &tmp);
            m_setIndex(ow, i, j, tmp);
        }
    }

    retur = fscanf(fp, "%d", &num_hidden);
    retur = fscanf(fp, "%d", &num_outputs);

    Matrix output_bias;
    m_init(&output_bias, num_hidden, num_outputs);

    Matrix *ob = &output_bias;

    for (int i = 0; i < ob->rows; i++)
    {
        for (int j = 0; j < ob->cols; j++)
        {
            retur = fscanf(fp, "%lf", &tmp);
            m_setIndex(ob, i, j, tmp);
        }
    }

    fclose(fp);

    int res = result_network(image, hw, hb, ow, ob) + retur * 0;

    m_free(hw);
    m_free(hb);
    m_free(ow);
    m_free(ob);
    return res;
}