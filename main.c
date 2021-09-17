#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "image_processing/image_processing.h"
#include "utils/image.h"

static void print_help(const char *exec_name)
{
    printf("Usage: %s [options] file ...\n", exec_name);
    printf("Options:\n");
    printf("\t-o <file>     Place the output into <file>.\n");
    printf("\n");
    printf("For more information, see: "
           "https://github.com/augustinbegue/sudoku-ocr\n");
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

    char *input_path = "", *output_path = "./output.bmp";

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            // next argument is the output_path
            i++;
            output_path = (char *)argv[i];
            continue;
        }
        else
        {
            input_path = (char *)argv[i];
        }
    }

    // input_path contains the file path -> load it
    if (access(input_path, F_OK) == 0)
    {
        SDL_Surface *image_surface = load_image(input_path);

        Image image = SDL_Surface_to_Image(image_surface);

        // save the image in the output_path file
        save_image(image_surface, output_path);

        free_Image(&image);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}
