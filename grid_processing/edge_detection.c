#include <math.h>
#include <stdbool.h>
#include "../image_processing/blur.h"
#include "../image_processing/filters.h"
#include "../image_processing/threshold.h"
#include "../utils/helpers.h"
#include "../utils/image.h"

// clang-format off
double sobel_kernel_x[9] = {
    1, 0, -1, 
    2, 0, -2, 
    1, 0, -1
    };

double sobel_kernel_y[9] = {
     1,  2,  1,
     0,  0,  0,
    -1, -2, -1
    };
// clang-format on

#define WEAK_EDGE_VAL 128
#define STRONG_EDGE_VAL 255

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
static void convolution(double *kernel, int ksize, Image *image, Image *out)
{
    const int khalf = ksize / 2;

    int w = image->height;
    int h = image->width;

    for (int m = khalf; m < w - khalf; m++)
    {
        for (int n = khalf; n < h - khalf; n++)
        {
            float pixel = 0.0;
            size_t c = 0;
            for (int j = -khalf; j <= khalf; j++)
            {
                for (int i = -khalf; i <= khalf; i++)
                {
                    pixel += clamp(
                        image->pixels[n - j][m - i].r * kernel[c], 0, 255);
                    c++;
                }
            }

            Pixel pix = {pixel, pixel, pixel};
            out->pixels[n][m] = pix;
        }
    }
}

static void generate_gradient_image(
    Image *image, Image *xgradient, Image *ygradient, Image *magnitude)
{
    int w = image->width;
    int h = image->height;

    convolution(sobel_kernel_x, 3, image, xgradient);

    convolution(sobel_kernel_y, 3, image, ygradient);

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

void find_edges_image(Image *source, bool verbose_mode, char *verbose_path)
{
    if (verbose_mode)
        printf("\n[2]📐 Detecting Edges...\n");

    Image image = clone_image(source);

    if (verbose_mode)
        printf("   🔎 Blurring the image.\n");

    int gkernel_size = 25;
    double *gaussian_kernel = get_gaussian_smoothing_kernel(gkernel_size, 100);
    convolution(gaussian_kernel, gkernel_size, source, &image);
    free(gaussian_kernel);

    verbose_save(verbose_mode, verbose_path, "6-edges-blur.png", &image);

    if (verbose_mode)
        printf("   📈 Generating the intensity gradient...\n");

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
    if (verbose_mode)
        printf("   🧨 Non-Maximum suppression...\n");

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

            double dir
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

    // Double Threshold
    if (verbose_mode)
        printf("   🧺 Double-Threshold filtering...\n");

    // Threshold determination
    int *hist = image_grayscale_histogram(&gradient, 0, w, 0, h);
    int high_threshold = get_histogram_threshold(hist);
    int low_threshold = high_threshold / 2;
    free(hist);

    if (verbose_mode)
    {
        printf("   👇 Low threshold: %i\n", low_threshold);
        printf("   👆 High threshold: %i\n", high_threshold);
    }

    // Threshold application
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int val = gradient.pixels[x][y].r;

            if (val == 0)
                continue;

            if (val <= low_threshold)
                val = 0;
            else if (low_threshold <= val && val <= high_threshold)
                val = WEAK_EDGE_VAL;
            else
                val = STRONG_EDGE_VAL;

            Pixel pix = {val, val, val};
            gradient.pixels[x][y] = pix;
        }
    }

    verbose_save(
        verbose_mode, verbose_path, "6.4-edges-filtered.png", &gradient);

    // Hysterisis Analysis
    if (verbose_mode)
        printf("   🕳️ Hysterisis Analysis...\n");

    for (int x = 2; x < w - 2; x++)
    {
        for (int y = 2; y < h - 2; y++)
        {
            int val = gradient.pixels[x][y].r;

            if (val != WEAK_EDGE_VAL)
                continue;

            for (int i = x - 2; i <= x + 2; i++)
            {
                for (int j = y - 2; j < y + 2; j++)
                {
                    if (x == i && y == j)
                        continue;

                    if (gradient.pixels[i][j].r == STRONG_EDGE_VAL)
                    {
                        gradient.pixels[x][y].r = gradient.pixels[x][y].g
                            = gradient.pixels[x][y].b = STRONG_EDGE_VAL;
                        break;
                    }
                }
            }

            gradient.pixels[x][y].r = gradient.pixels[x][y].g
                = gradient.pixels[x][y].b = 0;
        }
    }

    verbose_save(verbose_mode, verbose_path, "6.5-edges-final.png", &gradient);

    free_Image(&gradient);
}
