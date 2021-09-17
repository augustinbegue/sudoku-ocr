#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

void init_sdl();

SDL_Surface *load_image(char *path);

void save_image(SDL_Surface *image_surface, char *path);

#endif