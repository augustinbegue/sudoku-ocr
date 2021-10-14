#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

struct Pixel
{
    Uint8 r, g, b;
};
typedef struct Pixel Pixel;

struct Image
{
    int height;
    int width;
    SDL_Surface *surface;
    Pixel **pixels;
    double average_color;
};
typedef struct Image Image;

SDL_Surface *load_image(char *path);

void save_image(SDL_Surface *image_surface, char *path);

Image clone_image(Image *source);

Image SDL_Surface_to_Image(SDL_Surface *image_surface);

SDL_Surface *Image_to_SDL_Surface(Image *image);

void free_Image(Image *image);

void image_filter(Image *image, void (*filter)(Pixel *, int), double value);

void image_partial_filter(Image *image, void (*filter)(Pixel *, int),
    double value, int startx, int endx, int starty, int endy);

int *image_grayscale_histogram(
    Image *image, int startx, int endx, int starty, int endy);

#endif