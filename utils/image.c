#include <err.h>
#include <stdio.h>
#include "./image.h"
#include "./pixel_operations.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Surface *load_image(char *path)
{
    printf("-->📁 Opening %s\n", path);

    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

void save_image(SDL_Surface *image_surface, char *path)
{
    printf("<--💾 Saving output to %s\n", path);
    int success = SDL_SaveBMP(image_surface, path);

    if (success != 0)
        errx(1, "could not save the image to '%s': %s.\n", path,
            SDL_GetError());
}

Image SDL_Surface_to_Image(SDL_Surface *image_surface)
{
    Image image;

    image.height = image_surface->h;
    image.width = image_surface->w;
    image.surface = image_surface;
    image.pixels = malloc((image.width + 1) * sizeof(Pixel *));
    image.average_color = 0;

    double pnum = image.width * image.height;

    if (image.pixels == NULL)
    {
        errx(1, "Error when allocating memory in SDL_SurfaceToImage.");
    }

    for (int x = 0; x < image.width; x++)
    {
        image.pixels[x] = malloc((image.height + 1) * sizeof(Pixel));

        if (image.pixels[x] == NULL)
        {
            errx(1, "Error when allocating memory in SDL_SurfaceToImage.");
        }

        SDL_Color values;

        for (int y = 0; y < image.height; y++)
        {
            Uint32 tpixel = get_pixel(image.surface, x, y);

            SDL_GetRGB(tpixel, image.surface->format, &values.r, &values.g,
                &values.b);

            Pixel pixel = {values.r, values.g, values.b};

            double c_avg = (values.r + values.g + values.b) / 3;

            image.average_color += c_avg / pnum;

            image.pixels[x][y] = pixel;
        }
    }

    return image;
}

SDL_Surface *Image_to_SDL_Surface(Image *image)
{
    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            Pixel p = image->pixels[x][y];

            put_pixel(image->surface, x, y,
                SDL_MapRGB(image->surface->format, p.r, p.g, p.b));
        }
    }

    return image->surface;
}

void free_Image(Image *image)
{
    for (int i = 0; i < image->width; i++)
    {
        free(image->pixels[i]);
    }

    free(image->pixels);

    SDL_FreeSurface(image->surface);
}

void image_filter(Image *image, void (*filter)(Pixel *, int), double value)
{
    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            filter(&image->pixels[x][y], value);
        }
    }
}

// Array is size 256
Uint8 *image_grayscale_histogram(Image *image)
{
    Uint8 *hist = calloc(sizeof(Uint8), 256);

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            hist[image->pixels[x][y].r]++;
        }
    }

    return hist;
}