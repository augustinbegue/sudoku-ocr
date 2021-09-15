#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "image_processing/image_manager.h"

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        errx(1, "missing operand.\nTry --help for more information.");
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        // --help argument -> print help message

        // TODO: Write the help message
        printf("Help message.\n");
    }

    char *input_path = "", *output_path = "";

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

        // save the image in the output_path file
        save_image(image_surface, output_path);
    }
    else
    {
        // File doesn't exist.
        errx(1, "cannot open '%s': No such file or directory.", input_path);
    }

    return 0;
}
