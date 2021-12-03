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
    Image image
       = SDL_Surface_to_Image(load_image("assets/grids/blank-grid.png"));

    

    save_image(Image_to_SDL_Surface(&image),
        "sudoku_solver/tes.png");
    return 0;
}
