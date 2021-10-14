#include <float.h>
#include <math.h>
#include <stdbool.h>
#include "../image_processing/blur.h"
#include "../image_processing/filters.h"
#include "../utils/helpers.h"
#include "../utils/image.h"

// clang-format off
int sobel_kernel_x[9] = {
    -1, 0, 1, 
    -2, 0, 2, 
    -1, 0, 1
    };

int sobel_kernel_y[9] = {
     1,  2,  1,
     0,  0,  0,
    -1, -2, -1
    };
// clang-format oon
/**
 * @brief convolution operator
 *
 * @link https://en.wikipedia.org/wiki/Kernel_(image_processing)#Convolution
 *
 * @param kernel kernel to apply
 * @param kernel_size size of the kernel (should be odd)
 * @param x x coordinate of the pixel to convolute
 * @param y y coordinate of the pixel to convolute
 * @param image image to take samples from
 * @return Pixel pixel to return
 */
static void convolution(int kernel[9], Image *image, Image *out)
{
    const int ksize = 3;
    const int khalf = ksize / 2;
    float min = FLT_MAX, max = -FLT_MAX;

    int w = image->height;
    int h = image->width;

    for (int m = khalf; m < w - khalf; m++)
    {
        for (int n = khalf; n < h - khalf; n++)
        {
            float pixel = 0.0;
            size_t c = 0;
            for (int j = -khalf; j <= khalf; j++) {
                for (int i = -khalf; i <= khalf; i++)
                {
                    pixel += image->pixels[n - j][m - i].r * kernel[c];
                    c++;
                }
            }

            if (pixel < min)
                min = pixel;

            if (pixel > max)
                max = pixel;
        }
    }

    for (int m = khalf; m < w - khalf; m++)
    {
        for (int n = khalf; n < h - khalf; n++)
        {
            float pixel = 0.0;
            size_t c = 0;
            for (int j = -khalf; j <= khalf; j++) {
                for (int i = -khalf; i <= khalf; i++)
                {
                    pixel += image->pixels[n - j][m - i].r * kernel[c];
                    c++;
                }
            }
            pixel = 255 * (pixel - min) / (max - min);
            out->pixels[n][m].r = out->pixels[n][m].g = out->pixels[n][m].b = pixel;
        }
    }

    // int accumulator = 0;
    // int w = image->width;
    // int h = image->height;

    // for (int xk = 0; xk < 3; xk++)
    // {
    //     for (int yk = 0; yk < 3; yk++)
    //     {
    //         int element = kernel[xk][yk];

    //         int xs = xk + x;
    //         int ys = yk + y;

    //         if (0 <= xs && xs < w && 0 <= ys && ys < h)
    //             accumulator += image->pixels[xs][ys].r * element;
    //     }
    // }

    // Pixel pix = {accumulator, accumulator, accumulator};
    // return pix;
}

static void generate_gradient_image(
    Image *image, Image *xgradient, Image *ygradient, Image *magnitude)
{
    int w = image->width;
    int h = image->height;

    convolution(sobel_kernel_x, image, xgradient);

    convolution(sobel_kernel_y, image, ygradient);

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int magnitudeval, xval = xgradient->pixels[x][y].r,
                              yval = ygradient->pixels[x][y].r;

            magnitudeval = clamp(sqrt(xval * xval + yval * yval), 0, 255);

            Pixel pix = {magnitudeval, magnitudeval, magnitudeval};
            magnitude->pixels[x][y] = pix;
        }
    }
}

void find_edge_image(Image *source, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("\n[2]📐 Detecting Edges.\n");

    Image image = clone_image(source);

    if (verbose_mode)
        printf("   🔎 Blurring the image.\n");

    gaussian_blur_image(&image, 9, 10, 1.5);

    verbose_save(verbose_mode, verbose_path, "6-edges-blur.png", &image);

    if (verbose_mode)
        printf("   📈 Generating the intensity gradient.\n");

    Image xgradient = clone_image(&image);
    Image ygradient = clone_image(&image);
    Image gradient = clone_image(&image);

    generate_gradient_image(&image, &xgradient, &ygradient, &gradient);
    free_Image(&image);

    verbose_save(verbose_mode, verbose_path, "6.1-x-gradient.png", &xgradient);
    verbose_save(verbose_mode, verbose_path, "6.1-y-gradient.png", &ygradient);
    verbose_save(
        verbose_mode, verbose_path, "6.2-magnitude-gradient.png", &gradient);

    int w = gradient.width;
    int h = gradient.height;

    // Non-maximum suppression
    for (int x = 1; x < w - 1; x++)
    {
        for (int y = 1; y < h - 1; y++)
        {
            Pixel *c = &gradient.pixels[x][y];
            Pixel *nn = &gradient.pixels[x][y - 1];
            Pixel *ss = &gradient.pixels[x][y + 1];
            Pixel *ww = &gradient.pixels[x + 1][y];
            Pixel *ee = &gradient.pixels[x - 1][y];
            Pixel *nw = &gradient.pixels[x + 1][y - 1];
            Pixel *ne = &gradient.pixels[x - 1][y - 1];
            Pixel *sw = &gradient.pixels[x + 1][y + 1];
            Pixel *se = &gradient.pixels[x - 1][y + 1];

            int dir
                = atan2(xgradient.pixels[x][y].r, ygradient.pixels[x][y].r);

            if (!(((dir <= 1 || dir > 7) && c->r > ee->r && c->r > ww->r)
                    || // 0 deg
                    ((dir > 1 && dir <= 3) && c->r > nw->r && c->r > se->r)
                    || // 45 deg
                    ((dir > 3 && dir <= 5) && c->r > nn->r && c->r > ss->r)
                    || // 90 deg
                    ((dir > 5 && dir <= 7) && c->r > ne->r
                        && c->r > sw->r))) // 135 deg
                c->r = c->g = c->b = 0;
        }
    }

    free_Image(&xgradient);
    free_Image(&ygradient);

    verbose_save(
        verbose_mode, verbose_path, "6.3-edges-gradient.png", &gradient);

    free_Image(&gradient);
}
