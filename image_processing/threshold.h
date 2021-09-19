#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <err.h>
#include <stdio.h>
#include "../utils/image.h"

Uint8 get_histogram_threshold(int *histogram);

#endif