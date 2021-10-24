#include <float.h>
#include <math.h>
#include <stdbool.h>
#include "blur.h"
#include "filters.h"
#include "helpers.h"
#include "image.h"
#include "int_list.h"
#include "morph.h"
#include "threshold.h"

#define MAX_BRIGHTNESS 255
#define T_LOW 0.4
#define T_HIGH 0.75
#define SUPPRESSED 0
#define WEAK_EDGE_VAL 128
#define STRONG_EDGE_VAL MAX_BRIGHTNESS

static void histogram(int *buf, int w, int h, int *histo)
{
    int bufLength = w * h;

    for (int i = 0; i < bufLength; ++i)
    {
        histo[buf[i]]++;
    }
}

/**
 * @brief Computes the intensity gradients per pixel and per axis of an image
 *
 * @param in int array containing the image's pixels
 * @param w image width
 * @param h image height
 * @param deltaX x axis intensity gradient
 * @param deltaY y axis intensity gradient
 */
static void compute_gradients(int *in, int w, int h, int *deltaX, int *deltaY)
{
    int i, j;
    int t;

    // compute delta X ***************************
    // deltaX = f(x+1) - f(x-1)
    for (i = 0; i < h; ++i)
    {
        t = i * w; // current position X per line

        // gradient at the first pixel of each line
        // note: the edge,pix[t-1] is NOT exsit
        deltaX[t] = in[t + 1] - in[t];

        // gradients where NOT edge
        for (j = 1; j < w - 1; ++j)
        {
            t++;
            deltaX[t] = in[t + 1] - in[t - 1];
        }

        // gradient at the last pixel of each line
        t++;
        deltaX[t] = in[t] - in[t - 1];
    }

    // compute delta Y ***************************
    // deltaY = f(y+1) - f(y-1)
    for (j = 0; j < w; ++j)
    {
        t = j; // current Y position per column

        // gradient at the first pixel
        deltaY[t] = in[t + w] - in[t];

        // gradients for NOT edge pixels
        for (i = 1; i < h - 1; ++i)
        {
            t += w;
            deltaY[t] = in[t + w] - in[t - w];
        }

        // gradient at the last pixel of each column
        t += w;
        deltaY[t] = in[t] - in[t - w];
    }
}

/**
 * @brief Computes the magnitude gradient of an image from its intensity
 * gradients
 *
 * @param deltaX  x axis intensity gradient
 * @param deltaY  y axis intensity gradient
 * @param w width of the image
 * @param h height of the image
 * @param mag magnitude gradient
 */
static void compute_magnitude(int *deltaX, int *deltaY, int w, int h, int *mag)
{
    int i, j, t;

    t = 0;
    for (i = 0; i < h; ++i)
        for (j = 0; j < w; ++j, ++t)
            mag[t] = (int)(sqrt((double)deltaX[t] * deltaX[t]
                                + (double)deltaY[t] * deltaY[t])
                           + 0.5);
}

/**
 * @brief Applies a NMS (Non Maximal Suppression) algorithm on a magnitude
 * gradient
 *
 * @param magnitude magnitude gradient
 * @param deltaX  x axis intensity gradient
 * @param deltaY  y axis intensity gradient
 * @param w width of the image
 * @param h height of the image
 * @param nms non maximal supressed gradient
 */
static void non_maximal_suppression(
    int *magnitude, int *deltaX, int *deltaY, int w, int h, int *nms)
{
    int i, j, t;
    float alpha;
    float mag1, mag2;

    // put zero all boundaries of image
    // TOP edge line of the image
    for (j = 0; j < w; ++j)
        nms[j] = 0;

    // BOTTOM edge line of image
    t = (h - 1) * w;
    for (j = 0; j < w; ++j, ++t)
        nms[t] = 0;

    // LEFT & RIGHT edge line
    t = w;
    for (i = 1; i < h; ++i, t += w)
    {
        nms[t] = 0;
        nms[t + w - 1] = 0;
    }

    t = w + 1; // skip boundaries of image
    // start and stop 1 pixel inner pixels from boundaries to avoid segfault
    for (i = 1; i < h - 1; i++, t += 2)
    {
        for (j = 1; j < w - 1; j++, t++)
        {
            // if magnitude = 0, no edge
            if (magnitude[t] == 0)
                nms[t] = SUPPRESSED;
            else
            {
                /*
                 * looking for 8 directions (clockwise)
                 * NOTE: positive dy means down(south) direction on
                 * screen(image) because dy is defined as f(x,y+1)-f(x,y-1). 1:
                 * dy/dx <= 1,  dx > 0, dy > 0 2: dy/dx > 1,   dx > 0, dy > 0
                 * 3: dy/dx >= -1, dx < 0, dy > 0
                 * 4: dy/dx < -1,  dx < 0, dy > 0
                 * 5: dy/dx <= 1,  dx < 0, dy < 0
                 * 6: dy/dx > 1,   dx < 0, dy < 0
                 * 7: dy/dx <= -1, dx > 0, dy < 0
                 * 8: dy/dx > -1,  dx > 0, dy < 0
                 * After determine direction,
                 * compute magnitudes of neighbour pixels using linear
                 * interpolation
                 */
                if (deltaX[t] >= 0)
                {
                    if (deltaY[t] >= 0) // dx >= 0, dy >= 0
                    {
                        if ((deltaX[t] - deltaY[t]) >= 0) // direction 1 (SEE)
                        {
                            alpha = (float)deltaY[t] / deltaX[t];
                            mag1 = (1 - alpha) * magnitude[t + 1]
                                   + alpha * magnitude[t + w + 1];
                            mag2 = (1 - alpha) * magnitude[t - 1]
                                   + alpha * magnitude[t - w - 1];
                        }
                        else // direction 2 (SSE)
                        {
                            alpha = (float)deltaX[t] / deltaY[t];
                            mag1 = (1 - alpha) * magnitude[t + w]
                                   + alpha * magnitude[t + w + 1];
                            mag2 = (1 - alpha) * magnitude[t - w]
                                   + alpha * magnitude[t - w - 1];
                        }
                    }

                    else // dx >= 0, dy < 0
                    {
                        if ((deltaX[t] + deltaY[t]) >= 0) // direction 8 (NEE)
                        {
                            alpha = (float)-deltaY[t] / deltaX[t];
                            mag1 = (1 - alpha) * magnitude[t + 1]
                                   + alpha * magnitude[t - w + 1];
                            mag2 = (1 - alpha) * magnitude[t - 1]
                                   + alpha * magnitude[t + w - 1];
                        }
                        else // direction 7 (NNE)
                        {
                            alpha = (float)deltaX[t] / -deltaY[t];
                            mag1 = (1 - alpha) * magnitude[t + w]
                                   + alpha * magnitude[t + w - 1];
                            mag2 = (1 - alpha) * magnitude[t - w]
                                   + alpha * magnitude[t - w + 1];
                        }
                    }
                }

                else
                {
                    if (deltaY[t] >= 0) // dx < 0, dy >= 0
                    {
                        if ((deltaX[t] + deltaY[t]) >= 0) // direction 3 (SSW)
                        {
                            alpha = (float)-deltaX[t] / deltaY[t];
                            mag1 = (1 - alpha) * magnitude[t + w]
                                   + alpha * magnitude[t + w - 1];
                            mag2 = (1 - alpha) * magnitude[t - w]
                                   + alpha * magnitude[t - w + 1];
                        }
                        else // direction 4 (SWW)
                        {
                            alpha = (float)deltaY[t] / -deltaX[t];
                            mag1 = (1 - alpha) * magnitude[t - 1]
                                   + alpha * magnitude[t + w - 1];
                            mag2 = (1 - alpha) * magnitude[t + 1]
                                   + alpha * magnitude[t - w + 1];
                        }
                    }

                    else // dx < 0, dy < 0
                    {
                        if ((-deltaX[t] + deltaY[t]) >= 0) // direction 5 (NWW)
                        {
                            alpha = (float)deltaY[t] / deltaX[t];
                            mag1 = (1 - alpha) * magnitude[t - 1]
                                   + alpha * magnitude[t - w - 1];
                            mag2 = (1 - alpha) * magnitude[t + 1]
                                   + alpha * magnitude[t + w + 1];
                        }
                        else // direction 6 (NNW)
                        {
                            alpha = (float)deltaX[t] / deltaY[t];
                            mag1 = (1 - alpha) * magnitude[t - w]
                                   + alpha * magnitude[t - w - 1];
                            mag2 = (1 - alpha) * magnitude[t + w]
                                   + alpha * magnitude[t + w + 1];
                        }
                    }
                }

                // non-maximal suppression
                // compare mag1, mag2 and mag[t]
                // if mag[t] is smaller than one of the neighbours then
                // suppress it
                if ((magnitude[t] < mag1) || (magnitude[t] < mag2))
                    nms[t] = SUPPRESSED;
                else
                {
                    // Normalize values above 255
                    if (magnitude[t] > 255)
                        nms[t] = 255;
                    else
                        nms[t] = (unsigned char)magnitude[t];
                }

            } // END OF ELSE (mag != 0)
        }     // END OF FOR(j)
    }         // END OF FOR(i)
}

static void trace_edge_find_neighbours(
    int *in, int t, int w, int threshold, int *out, int_list *edges)
{
    int nw, no, ne, we, ea, sw, so, se; // indice of 8 neighbours

    // get indice of 8 neighbours
    nw = t - w - 1; // north-west
    no = nw + 1;    // north
    ne = no + 1;    // north-east
    we = t - 1;     // west
    ea = t + 1;     // east
    sw = t + w - 1; // south-west
    so = sw + 1;    // south
    se = so + 1;    // south-east

    // initially, test 8 neighbours and add them into edge list only if they
    // are also edges
    if (in[nw] >= threshold && out[nw] != STRONG_EDGE_VAL)
    {
        out[nw] = STRONG_EDGE_VAL;
        li_append(edges, nw);
    }
    if (in[no] >= threshold && out[no] != STRONG_EDGE_VAL)
    {
        out[no] = STRONG_EDGE_VAL;
        li_append(edges, no);
    }
    if (in[ne] >= threshold && out[ne] != STRONG_EDGE_VAL)
    {
        out[ne] = STRONG_EDGE_VAL;
        li_append(edges, ne);
    }
    if (in[we] >= threshold && out[we] != STRONG_EDGE_VAL)
    {
        out[we] = STRONG_EDGE_VAL;
        li_append(edges, we);
    }
    if (in[ea] >= threshold && out[ea] != STRONG_EDGE_VAL)
    {
        out[ea] = STRONG_EDGE_VAL;
        li_append(edges, ea);
    }
    if (in[sw] >= threshold && out[sw] != STRONG_EDGE_VAL)
    {
        out[sw] = STRONG_EDGE_VAL;
        li_append(edges, sw);
    }
    if (in[so] >= threshold && out[so] != STRONG_EDGE_VAL)
    {
        out[so] = STRONG_EDGE_VAL;
        li_append(edges, so);
    }
    if (in[se] >= threshold && out[se] != STRONG_EDGE_VAL)
    {
        out[se] = STRONG_EDGE_VAL;
        li_append(edges, se);
    }
}

/**
 * @brief follows edges and draws them
 *
 * @param in input image
 * @param t index to start at
 * @param w width of the image
 * @param threshold threshold to detect neighbours
 * @param out output image
 */
static void trace_edge(int *in, int t, int w, int threshold, int *out)
{
    int_list *edges = li_create(); // list of edges to be checked

    trace_edge_find_neighbours(in, t, w, threshold, out, edges);

    // loop until all edge candiates are tested
    while (!li_empty(edges))
    {
        t = edges->tail->value;
        li_pop(edges); // remove the last after read

        trace_edge_find_neighbours(in, t, w, threshold, out, edges);
    }

    li_free(edges);
}

/**
 * @brief Applies hysterisis analysis on an image to select strong edges
 *
 * @link
 * https://en.wikipedia.org/wiki/Canny_edge_detector#Edge_tracking_by_hysteresis
 *
 * @param gradient gradient to apply the filter on
 * @param w width of the image
 * @param h height of the image
 */
static void hysterisis_analysis(int *in, int w, int h, float tLow, float tHigh,
    int *out, bool verbose_mode)
{
    int i, j, t;
    int iMax, jMax;
    int histo[256] = {0};
    int sum, highCount, lowCount;
    int lowValue, highValue;

    // clear output (set all to 0)
    memset(out, (char)0, sizeof(int) * w * h);

    // get histogram to compute high & low level trigger values
    histogram(in, w, h, histo);

    // find the number of pixels where tHigh(percentage) of total pixels except
    // zeros
    highCount = (int)((w * h - histo[0]) * tHigh + 0.5f);

    // compute high level trigger value using histogram distribution
    i = 255;                // max value in unsigned char
    sum = w * h - histo[0]; // total sum of histogram array except zeros
    while (sum > highCount)
        sum -= histo[i--];
    highValue = (unsigned char)++i;

    // find the number of pixels where tLow(percentage) of total pixels except
    // zeros
    lowCount = (int)((w * h - histo[0]) * tLow + 0.5f);

    // compute low trigger value using histogram distribution
    i = 0;
    sum = 0;
    while (sum < lowCount)
        sum += histo[i++];
    lowValue = (unsigned char)i;
    lowValue = (int)(highValue * tLow + 0.5f);

    // search a pixel which is greater than high level trigger value
    // then mark it as an edge and start to trace edges using low trigger value
    // If the neighbors are greater than low trigger, they become edges
    // NOTE: Use general thresholding method for faster computation, but
    //       hysteresis thresholding eliminates noisy pixels and connects
    //       the diconnected edges.
    t = w + 1;    // skip the first line
    iMax = h - 1; // max Y value to traverse, ignore bottom border
    jMax = w - 1; // max X value to traverse, ignore right border

    if (verbose_mode)
    {
        printf("   👆 High Treshold: %f, High Trigger: %i\n", tHigh, highValue);
        printf("   👇 Low Treshold: %f, Low Trigger: %i\n", tLow, lowValue);
    }

    for (i = 1; i < iMax; ++i)
    {
        for (j = 1; j < jMax; ++j, ++t)
        {
            if (in[t] >= highValue
                && out[t]
                       != STRONG_EDGE_VAL) // trace only if not checked before
            {
                out[t] = STRONG_EDGE_VAL; // set itself as an edge
                trace_edge(in, t, w, lowValue,
                    out); // start tracing from this point
            }
        }
    }
}

Image canny_edge_filtering(
    Image *source, bool verbose_mode, char *verbose_path)
{
    int w = source->width;
    int h = source->height;

    if (verbose_mode)
        printf("\n[2]📐 Detecting Edges...\n");

    Image image = clone_image(source);

    /*
     * Gaussian Smoothing
     */
    if (verbose_mode)
        printf("   🔎 Blurring the image.\n");

    int gkernel_size = 11;
    double *gaussian_kernel = get_gaussian_smoothing_kernel(gkernel_size, 100);
    convolution(gaussian_kernel, gkernel_size, source, &image, false);
    free(gaussian_kernel);

    verbose_save(verbose_mode, verbose_path, "6.0-edges-blur.png", &image);

    if (verbose_mode)
        printf("   📈 Generating the intensity gradient...\n");
    else
        fprintf(stderr, "\33[2K\r[====================--------]");

    // Cloning images to store the gradients
    int *image_arr = Image_to_Array(&image);
    int *xgradient_arr = Image_to_Array(&image);
    int *ygradient_arr = Image_to_Array(&image);
    int *magnitude_arr = Image_to_Array(&image);
    int *nms_arr = Image_to_Array(&image);
    int *edges_arr = Image_to_Array(&image);

    compute_gradients(image_arr, w, h, xgradient_arr, ygradient_arr);

    compute_magnitude(xgradient_arr, ygradient_arr, w, h, magnitude_arr);

    // Computing images for visualisation
    if (verbose_mode)
    {
        Image xgradient = clone_image(&image);
        Array_to_Image(xgradient_arr, &xgradient);
        Image ygradient = clone_image(&image);
        Array_to_Image(ygradient_arr, &ygradient);
        Image magnitude = clone_image(&image);
        Array_to_Image(magnitude_arr, &magnitude);

        verbose_save(
            verbose_mode, verbose_path, "6.1-x-gradient.png", &xgradient);
        verbose_save(
            verbose_mode, verbose_path, "6.1-y-gradient.png", &ygradient);
        verbose_save(verbose_mode, verbose_path, "6.2-magnitude-gradient.png",
            &magnitude);

        free_Image(&xgradient);
        free_Image(&ygradient);
        free_Image(&magnitude);
    }
    free(image_arr);

    /*
     * Non-maximal suppression
     */
    if (verbose_mode)
        printf("   🧨 Non-Maximal suppression...\n");
    else
        fprintf(stderr, "\33[2K\r[=====================-------]");

    non_maximal_suppression(
        magnitude_arr, xgradient_arr, ygradient_arr, w, h, nms_arr);

    Image edge_image = clone_image(&image);
    Array_to_Image(nms_arr, &edge_image);
    verbose_save(verbose_mode, verbose_path, "6.3-edges-nms.png", &edge_image);

    // Hysterisis Analysis
    if (verbose_mode)
        printf("   🕳️ Hysterisis Analysis...\n");
    else
        fprintf(stderr, "\33[2K\r[======================------]");

    hysterisis_analysis(nms_arr, w, h, T_LOW, T_HIGH, edges_arr, verbose_mode);

    Array_to_Image(edges_arr, &edge_image);
    verbose_save(
        verbose_mode, verbose_path, "6.4-edges-final.png", &edge_image);

    free(xgradient_arr);
    free(ygradient_arr);
    free(magnitude_arr);
    free(nms_arr);
    free(edges_arr);
    free_Image(&image);

    return edge_image;
}