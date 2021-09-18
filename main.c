#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "image_processing/image_filters.h"
#include "image_processing/otsu_threshold.h"
#include "utils/image.h"

static void print_help(const char *exec_name)
{
    printf("Usage: %s [options] file ...\n", exec_name);
    printf("Options:\n");
    printf("\t-o <file>     Save the output into <file>.\n");
    printf("\t-h <file>     Save the grayscale repartition histogram into "
           "<file>.\n");
    printf("\n");
    printf("For more information, see: "
           "https://github.com/augustinbegue/sudoku-ocr\n");
}

static void save_image_histogram(
    char *histogram_output_path, Uint8 *image_histogram)
{
    FILE *fpt;
    fpt = fopen(histogram_output_path, "w+");

    char num[7];
    int i = 0;
    for (; i < 255; i++)
    {

        sprintf(num, "%d;\n", image_histogram[i]);
        fputs(num, fpt);
    }

    sprintf(num, "%d\n", image_histogram[i]);
    fputs(num, fpt);

    printf("<--💾 Saving image grayscale histogram to: %s\n",
        histogram_output_path);

    fclose(fpt);
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        errx(1, "missing operand.\nTry --help for more information.");
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        // --help argument -> print help message
        print_help(argv[0]);
        return 0;
    }

    bool save_histogram = false;
    char *input_path = "", *output_path = "./output.bmp",
         *histogram_output_path = "";

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            // next argument is the output_path
            i++;
            output_path = (char *)argv[i];
            continue;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            save_histogram = true;
            // next argument is the output_path
            i++;
            histogram_output_path = (char *)argv[i];
            continue;
        }
        {
            input_path = (char *)argv[i];
        }
    }

    // input_path contains the file path -> load it
    if (access(input_path, F_OK) == 0)
    {
        Image image = SDL_Surface_to_Image(load_image(input_path));
        Image *imagept = &image;

        printf("[!]🎨 Average color: %f\n", image.average_color);

        filter_contrast(imagept, 128);

        filter_grayscale(imagept, 0);

        Uint8 *image_histogram = image_grayscale_histogram(imagept);
        if (save_histogram)
            save_image_histogram(histogram_output_path, image_histogram);

        double treshold = otsu_treshold(imagept, image_histogram);
        printf("[!]🔭 Otsu Treshold: %f\n", treshold);

        filter_bw(imagept, image.average_color);

        // save the image in the output_path file
        save_image(Image_to_SDL_Surface(imagept), output_path);

        free(image_histogram);
        free_Image(imagept);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}
