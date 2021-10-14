#include <stdbool.h>
#include "./image.h"

double clamp(double d, double min, double max)
{
    const double t = d < min ? min : d;
    return t > max ? max : t;
}

void verbose_save(
    bool verbose_mode, char *verbose_path, char *file_name, Image *image)
{
    if (verbose_mode)
    {
        char t[strlen(verbose_path) + strlen(file_name) + 2];
        strcpy(t, verbose_path);
        strcat(t, "/");
        strcat(t, file_name);
        save_image(Image_to_SDL_Surface(image), t);
    }
}