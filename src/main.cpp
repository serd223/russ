#include <math.h>

namespace SDL {
    #include <SDL3/SDL.h>
}

void drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) {
        SDL::SDL_WriteSurfacePixel(surface, x, y, color.r, color.g, color.b, color.a);
        if (D > 0) {
            x += xi;
            D += (2 * (dx - dy));
        } else {
            D += 2 * dx;
        }
    }
}

void drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {

    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        SDL::SDL_WriteSurfacePixel(surface, x, y, color.r, color.g, color.b, color.a);
        if (D > 0) {
            y += yi;
            D += (2 * (dy - dx));
        } else {
            D += 2 * dy;
        }
    }
}

void drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLow(surface, x1, x0, y1, y0, color);
        } else {
            drawLineLow(surface, x0, x1, y0, y1, color);
        }
    } else {
        if (y0 > y1) {
            drawLineHigh(surface, x1, x0, y1, y0, color);
        } else {
            drawLineHigh(surface, x0, x0, y0, y1, color);
        }
    }
}

int main() {
    SDL::SDL_Init(SDL_INIT_VIDEO);
    SDL::SDL_Window* win = SDL::SDL_CreateWindow("russ - dev", 800, 600, 0);

    SDL::SDL_Event event;
    SDL::SDL_Surface* surface = SDL::SDL_GetWindowSurface(win);

    drawLine(surface, 585, 15, 310, 310,  {255, 0, 0, 255});
    drawLine(surface, 15,  300, 50,  180, {0, 255, 0, 255});
    drawLine(surface, 585, 300, 50,  180, {0, 255, 0, 255});
    drawLine(surface, 15,  300, 310, 180, {0, 255, 0, 255});
    drawLine(surface, 585, 300, 310, 180, {0, 255, 0, 255});

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
