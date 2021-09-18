#ifndef OTSU_H
#define OTSU_H

#include <err.h>
#include <stdio.h>
#include "../utils/image.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

double otsu_treshold(Image *image, Uint8 *histogram);

#endif