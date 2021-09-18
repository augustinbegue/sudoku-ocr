#include <err.h>
#include <stdio.h>
#include "../utils/image.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

double otsu_treshold(Image *image, Uint8 *histogram)
{
    double total = image->width * image->height;
    double level = 0, sumB = 0, wB = 0;
    double maximum = 0;

    double sum1 = 0; // Scalar product between 0->255 and histogram
    for (int i = 0; i < 256; i++)
        sum1 += i * histogram[i];

    for (int j = 1; j < 256; j++)
    {
        double wF = total - wB;
        if (wB > 0 && wF > 0)
        {
            double mF = (sum1 - sumB) / wF;
            double val = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);

            if (val >= maximum)
            {
                level = j;
            }
        }

        wB += histogram[j];
        sumB += (j - 1) * histogram[j];
    }

    return level;
}
