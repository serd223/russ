#include <vector>
// #include <iostream>
namespace SDL {
    #include <SDL3/SDL.h>
}

void lineRasterisation(int x0, int x1, int y0, int y1, SDL::SDL_Surface* image) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int D = 2 * dy - dx;
    int y = y0;

    for (int x = x0; x < x1; x++) {
        SDL::SDL_WriteSurfacePixel(image, x, y, 0, 255, 0 , 255);
        if (D >= 0) {
            y++;
            D += 2 * (dy - dx);
        }
        else {
            D += 2 * dy;
        }
    }
}

int main() {
    SDL::SDL_Init(SDL_INIT_VIDEO);
    SDL::SDL_Window* win = SDL::SDL_CreateWindow("russ - dev", 800, 600, 0);

    SDL::SDL_Event event;
    SDL::SDL_Surface* surface = SDL::SDL_GetWindowSurface(win);
    lineRasterisation(15, 300, 20, 180, surface);

    for (;;) {
        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
        }
        SDL::SDL_UpdateWindowSurface(win);
    }
    loop_end:

    SDL::SDL_DestroyWindow(win);
    return 0;
}
