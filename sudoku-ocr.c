#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "image.h"
#include "pixel_operations.h"
#include "solver.h"

int main()
{
    init_sdl();
    // void replaceImage(Image * img, char *path, 0, 0);
    Image image
        = SDL_Surface_to_Image(load_image("../assets/grid/blank-grid.png"));

    return 0;
}