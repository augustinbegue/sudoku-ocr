#include <math.h>
#include "../utils/image.h"

void hough_transform(Image *image)
{
    int w = image->width, h = image->height;
    int diagonal = sqrt(w * w + h * h);

    int teta_min = 0, teta_max = 180;
    int rho_min = -diagonal, rho_max = diagonal;
}