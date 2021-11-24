#include <err.h>
#include <float.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "image.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "helpers.h"
#include "pixel_operations.h"

Image *create_image(int height, int width)
{
    Image *image = malloc(sizeof(Image));

    image->height = height;
    image->width = width;
    image->pixels = malloc(sizeof(Pixel *) * width + 1);

    if (image->pixels == NULL)
        errx(1, "Error when allocating memory in create_image.");

    for (int x = 0; x < width; x++)
        image->pixels[x] = malloc(sizeof(Pixel) * height + 1);

    image->surface = SDL_CreateRGBSurfaceWithFormat(
        0, width, height, 32, SDL_PIXELFORMAT_RGBA32);

    if (image->surface == NULL)
    {
        const char *errm = SDL_GetError();
        errx(-1, errm);
    }

    return image;
}

GdkPixbuf *image_to_pixbuf(Image *image)
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new(
        GDK_COLORSPACE_RGB, TRUE, 8, image->width, image->height);

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            guchar *p = pixels + y * rowstride + x * n_channels;
            p[0] = image->pixels[x][y].r;
            p[1] = image->pixels[x][y].g;
            p[2] = image->pixels[x][y].b;
            p[3] = 255;
        }
    }

    return pixbuf;
}

Image *pixbuf_to_image(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    Image *image = create_image(height, width);
    image->width = width;
    image->height = height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            guchar *p = pixels + y * rowstride + x * n_channels;
            image->pixels[x][y].r = p[0];
            image->pixels[x][y].g = p[1];
            image->pixels[x][y].b = p[2];
        }
    }

    return image;
}

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
    int success = IMG_SavePNG(image_surface, path);

    if (success != 0)
        errx(1, "could not save the image to '%s': %s.\n", path,
            SDL_GetError());
}

Image clone_image(Image *source)
{
    Image image;

    image.height = source->height;
    image.width = source->width;
    image.average_color = source->average_color;
    image.surface = SDL_ConvertSurface(
        source->surface, source->surface->format, SDL_SWSURFACE);
    image.pixels = malloc((image.width + 1) * sizeof(Pixel *));

    if (image.pixels == NULL)
    {
        errx(1, "Error when allocating memory in clone_image.");
    }

    for (int x = 0; x < image.width; x++)
    {
        image.pixels[x] = malloc((image.height + 1) * sizeof(Pixel));

        for (int y = 0; y < image.height; y++)
        {
            Pixel pSource = source->pixels[x][y];

            Pixel pDest = {pSource.r, pSource.g, pSource.b};

            image.pixels[x][y] = pDest;
        }
    }

    return image;
}

Image crop_image(Image *input, square *crop)
{
    point c1 = crop->c1;
    point c2 = crop->c2;
    point c3 = crop->c3;
    point c4 = crop->c4;

    double side1_length
        = sqrt((c2.x - c1.x) * (c2.x - c1.x) + (c2.y - c1.y) * (c2.y - c1.y));
    double side2_length
        = sqrt((c3.x - c2.x) * (c3.x - c2.x) + (c3.y - c2.y) * (c3.y - c2.y));
    double side3_length
        = sqrt((c4.x - c3.x) * (c4.x - c3.x) + (c4.y - c3.y) * (c4.y - c3.y));
    double side4_length
        = sqrt((c4.x - c1.x) * (c4.x - c1.x) + (c4.y - c1.y) * (c4.y - c1.y));

    int size;
    if (side1_length < side2_length)
        size = side2_length;
    else
        size = side1_length;

    if (side3_length > size)
        size = side3_length;

    if (side4_length > size)
        size = side4_length;

    point start;
    if (c1.x < c2.x)
        start.x = c1.x;
    else
        start.x = c2.x;
    if (c3.x < start.x)
        start.x = c3.x;
    if (c4.x < start.x)
        start.x = c4.x;

    if (c1.y < c2.y)
        start.y = c1.y;
    else
        start.y = c2.y;
    if (c3.y < start.y)
        start.y = c3.y;
    if (c4.y < start.y)
        start.y = c4.y;

    printf("   ✂️ Cropping Image - Start: (%i, %i), Size: %i\n", start.x,
        start.y, size);

    Image cropped;
    cropped.height = size;
    cropped.width = size;
    cropped.surface = SDL_CreateRGBSurfaceWithFormat(
        0, size, size, 32, SDL_PIXELFORMAT_RGBA32);
    cropped.pixels = malloc(sizeof(Pixel *) * size + 1);

    int old_x = start.x;
    for (int x = 0; x < size && old_x < input->width; x++, old_x++)
    {
        cropped.pixels[x] = malloc(sizeof(Pixel) * size + 1);

        int old_y = start.y;
        for (int y = 0; y < size && old_y < input->height; y++, old_y++)
        {
            Pixel old = input->pixels[old_x][old_y];
            cropped.pixels[x][y] = old;
        }
    }

    return cropped;
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

/**
 * @brief Converts a grayscale image into a unidimensional array
 *
 * @param source grayscale image
 * @return int*
 */
int *Image_to_Array(Image *source)
{
    int *out = malloc(sizeof(int) * source->width * source->height);
    int w = source->width, h = source->height;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            out[x + y * w] = source->pixels[x][y].r;
        }
    }

    return out;
}

/**
 * @brief Fills a container image from an array
 *
 * @param array int array to use of lenght width * height
 * @param container container image of size width * height
 * false
 */
void Array_to_Image(int *array, Image *container)
{
    int w = container->width, h = container->height;

    double max = 0;
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int val = array[x + y * w];

            if (val > max)
            {
                max = val;
            }
        }
    }

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            double val = (array[x + y * w] / max) * (double)255;

            Pixel pix = {val, val, val};
            container->pixels[x][y] = pix;
        }
    }
}

/**
 * @brief Creates an image from a 2d array
 *
 * @param array
 * @param width
 * @param height
 * @return Image
 */
Image Array2D_to_Image(int **array, int width, int height)
{
    Image image;
    image.width = width;
    image.height = height;
    image.pixels = malloc(sizeof(int *) * width + 1);
    image.surface = SDL_CreateRGBSurfaceWithFormat(
        0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    ;

    float min = FLT_MAX;
    float max = FLT_MIN;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int val = array[x][y];

            if (val < min)
                min = val;

            if (val > max)
                max = val;
        }
    }

    for (int x = 0; x < width; x++)
    {
        image.pixels[x] = malloc(sizeof(int) * height + 1);

        for (int y = 0; y < height; y++)
        {
            int pixel = array[x][y];

            pixel = 255 * (pixel - min) / (max - min);

            Pixel pix = {pixel, pixel, pixel};

            image.pixels[x][y] = pix;
        }
    }

    return image;
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
    double pnum = image->width * image->height;
    image->average_color = 0;

    for (int x = 0; x < image->width; x++)
    {
        for (int y = 0; y < image->height; y++)
        {
            filter(&image->pixels[x][y], value);

            double c_avg = (image->pixels[x][y].r + image->pixels[x][y].g
                               + image->pixels[x][y].b)
                           / 3;

            image->average_color += c_avg / pnum;
        }
    }
}

void image_partial_filter(Image *image, void (*filter)(Pixel *, int),
    double value, int startx, int endx, int starty, int endy)
{
    for (int x = startx; x < endx && x < image->width; x++)
    {
        for (int y = starty; y < endy && y < image->height; y++)
        {
            filter(&image->pixels[x][y], value);
        }
    }
}

/**
 * @brief Generates a grayscale repartition histogram from the image in
 * the specified positions
 *
 * @param image image to generate the histogram from
 * @param startx x start position
 * @param endx x end position
 * @param starty y start position
 * @param endy y end position
 * @return int*
 */
int *image_grayscale_histogram(
    Image *image, int startx, int endx, int starty, int endy)
{
    int *hist = calloc(sizeof(int), 256);

    for (int x = startx; x < endx && x < image->width; x++)
    {
        for (int y = starty; y < endy && y < image->height; y++)
        {
            hist[image->pixels[x][y].r]++;
        }
    }

    return hist;
}

/**
 * @brief Get the histogram minimum value's index
 *
 * @param hist
 * @return int
 */
int get_histogram_min(int *hist)
{
    int min = INT_MAX;
    int minColor = 0;

    for (int i = 0; i < 256; i++)
    {
        if (hist[i] < min)
        {
            min = hist[i];
            minColor = i;
        }
    }

    return minColor;
}

/**
 * @brief Get the histogram maximum value's index
 *
 * @param hist
 * @return int
 */
int get_histogram_max(int *hist)
{
    int max = INT_MIN;
    int maxColor = 0;

    for (int i = 0; i < 256; i++)
    {
        if (hist[i] > max)
        {
            max = hist[i];
            maxColor = i;
        }
    }

    return maxColor;
}

/**
 * @brief Draws a line from (x0, y0) to (x1, y1).
 *
 * @param image image to draw on
 * @param w width of the image
 * @param h height of the image
 * @param x0 starting point, x coordinate
 * @param y0 starting point, y coordinate
 * @param x1 end point, x coordinate
 * @param y1 end point, y coordinate
 * @param r red amount (0 -> 255)
 * @param g green amount (0 -> 255)
 * @param b blue amount (0 -> 255)
 * @return int** coordinates at which the line actually started and ended
 * (within the bound of the image).
 */
int *draw_line(Image *image, int w, int h, int x0, int y0, int x1, int y1,
    Uint8 r, Uint8 g, Uint8 b)
{
    Pixel color = {r, g, b};
    int *coordinates = malloc(sizeof(int) * 4 + 1);
    memset(coordinates, -1, sizeof(int) * 4);

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int err = dx + dy;

    while (true)
    {
        if (0 <= x0 && x0 < w && 0 <= y0 && y0 < h)
        {
            image->pixels[x0][y0] = color;
            if (coordinates[0] == -1 && coordinates[1] == -1)
            {
                coordinates[0] = x0;
                coordinates[1] = y0;
            }
            else
            {
                coordinates[2] = x0;
                coordinates[3] = y0;
            }
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }

    return coordinates;
}

/**
 * @brief Draws a square from the coordinates
 *
 * @param image
 * @param sqr
 * @param r
 * @param g
 * @param b
 */
void draw_square(Image *image, square *sqr, int r, int g, int b)
{
    free(draw_line(image, image->width, image->height, sqr->c1.x, sqr->c1.y,
        sqr->c2.x, sqr->c2.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c2.x, sqr->c2.y,
        sqr->c3.x, sqr->c3.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c3.x, sqr->c3.y,
        sqr->c4.x, sqr->c4.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c4.x, sqr->c4.y,
        sqr->c1.x, sqr->c1.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c1.x + 1,
        sqr->c1.y + 1, sqr->c2.x + 1, sqr->c2.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c2.x + 1,
        sqr->c2.y + 1, sqr->c3.x + 1, sqr->c3.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c3.x + 1,
        sqr->c3.y + 1, sqr->c4.x + 1, sqr->c4.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c4.x + 1,
        sqr->c4.y + 1, sqr->c1.x + 1, sqr->c1.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c1.x - 1,
        sqr->c1.y - 1, sqr->c2.x - 1, sqr->c2.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c2.x - 1,
        sqr->c2.y - 1, sqr->c3.x - 1, sqr->c3.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c3.x - 1,
        sqr->c3.y - 1, sqr->c4.x - 1, sqr->c4.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr->c4.x - 1,
        sqr->c4.y - 1, sqr->c1.x - 1, sqr->c1.y - 1, r, g, b));

    square _sqr2 = *sqr;
    square *sqr2 = &_sqr2;
    _sqr2.c1.x--;
    _sqr2.c1.y--;
    _sqr2.c2.x--;
    _sqr2.c2.y--;
    _sqr2.c3.x--;
    _sqr2.c3.y--;
    _sqr2.c4.x--;
    _sqr2.c4.y--;
    _sqr2.c1.x--;
    _sqr2.c1.y--;
    _sqr2.c2.x--;
    _sqr2.c2.y--;
    _sqr2.c3.x--;
    _sqr2.c3.y--;
    _sqr2.c4.x--;
    _sqr2.c4.y--;

    free(draw_line(image, image->width, image->height, sqr2->c1.x, sqr2->c1.y,
        sqr2->c2.x, sqr2->c2.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x, sqr2->c2.y,
        sqr2->c3.x, sqr2->c3.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x, sqr2->c3.y,
        sqr2->c4.x, sqr2->c4.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x, sqr2->c4.y,
        sqr2->c1.x, sqr2->c1.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c1.x + 1,
        sqr2->c1.y + 1, sqr2->c2.x + 1, sqr2->c2.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x + 1,
        sqr2->c2.y + 1, sqr2->c3.x + 1, sqr2->c3.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x + 1,
        sqr2->c3.y + 1, sqr2->c4.x + 1, sqr2->c4.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x + 1,
        sqr2->c4.y + 1, sqr2->c1.x + 1, sqr2->c1.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c1.x - 1,
        sqr2->c1.y - 1, sqr2->c2.x - 1, sqr2->c2.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x - 1,
        sqr2->c2.y - 1, sqr2->c3.x - 1, sqr2->c3.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x - 1,
        sqr2->c3.y - 1, sqr2->c4.x - 1, sqr2->c4.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x - 1,
        sqr2->c4.y - 1, sqr2->c1.x - 1, sqr2->c1.y - 1, r, g, b));

    _sqr2.c1.x++;
    _sqr2.c1.y++;
    _sqr2.c2.x++;
    _sqr2.c2.y++;
    _sqr2.c3.x++;
    _sqr2.c3.y++;
    _sqr2.c4.x++;
    _sqr2.c4.y++;
    _sqr2.c1.x++;
    _sqr2.c1.y++;
    _sqr2.c2.x++;
    _sqr2.c2.y++;
    _sqr2.c3.x++;
    _sqr2.c3.y++;
    _sqr2.c4.x++;
    _sqr2.c4.y++;

    free(draw_line(image, image->width, image->height, sqr2->c1.x, sqr2->c1.y,
        sqr2->c2.x, sqr2->c2.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x, sqr2->c2.y,
        sqr2->c3.x, sqr2->c3.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x, sqr2->c3.y,
        sqr2->c4.x, sqr2->c4.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x, sqr2->c4.y,
        sqr2->c1.x, sqr2->c1.y, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c1.x + 1,
        sqr2->c1.y + 1, sqr2->c2.x + 1, sqr2->c2.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x + 1,
        sqr2->c2.y + 1, sqr2->c3.x + 1, sqr2->c3.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x + 1,
        sqr2->c3.y + 1, sqr2->c4.x + 1, sqr2->c4.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x + 1,
        sqr2->c4.y + 1, sqr2->c1.x + 1, sqr2->c1.y + 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c1.x - 1,
        sqr2->c1.y - 1, sqr2->c2.x - 1, sqr2->c2.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c2.x - 1,
        sqr2->c2.y - 1, sqr2->c3.x - 1, sqr2->c3.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c3.x - 1,
        sqr2->c3.y - 1, sqr2->c4.x - 1, sqr2->c4.y - 1, r, g, b));
    free(draw_line(image, image->width, image->height, sqr2->c4.x - 1,
        sqr2->c4.y - 1, sqr2->c1.x - 1, sqr2->c1.y - 1, r, g, b));
}