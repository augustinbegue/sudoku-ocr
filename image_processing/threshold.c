#include <err.h>
#include <stdio.h>
#include "helpers.h"
#include "image.h"

Uint8 get_histogram_threshold_local_min(int *histogram)
{
    int firstmax = histogram[0];

    int i = 0;
    while (i < 256 && histogram[i] > firstmax)
    {
        firstmax = histogram[i];
        i++;
    }

    int firstmin = firstmax;
    while (i < 256 && firstmin > histogram[i])
    {
        firstmin = histogram[i];
        i++;
    }

    return i;
}

/**
 * @brief Get the histogram threshold otsu object
 *
 * @param histogram
 * @return Uint8
 */
Uint8 get_histogram_threshold_otsu(int *histogram)
{
    int nbpixel = 0;

    for (int i = 0; i < 256; i++)
        nbpixel += histogram[i];

    int sum1 = 0, sum2 = 0, w1 = 0, w2 = 0, m1 = 0, m2 = 0;
    double mid = 0, max = 0;
    Uint8 threshold = 0;

    int i;
    for (i = 0; i < 256; i++)
        sum1 += i * histogram[i];

    for (i = 0; i < 256; i++)
    {
        w1 += histogram[i];

        if (w1 == 0)
            continue;

        w2 = nbpixel - w1;

        if (w2 == 0)
            break;

        sum2 += i * histogram[i];
        m1 = sum2 / w1;
        m2 = (sum1 - sum2) / w2;
        mid = w1 * w2 * (m1 - m2) * (m1 - m2);

        if (mid >= max)
        {
            threshold = i;

            max = mid;
        }
    }

    return clamp(threshold, 70, 240);
}

/**
 * @brief Get the black and white threshold from an histogram
 *
 * @param nbpixel number of pixels in the image the histogram is from
 * @param histogram histogram from the image
 * @return Uint8
 */
Uint8 get_histogram_threshold(int *histogram)
{
    return get_histogram_threshold_otsu(histogram);
}
