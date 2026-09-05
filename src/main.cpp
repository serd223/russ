#include <math.h>
#include <stdio.h>

namespace SDL {
    #include <SDL3/SDL.h>
}

namespace Render {
    static void _drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
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

    static void _drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {

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

    static void _drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
        if (abs(y1 - y0) < abs(x1 - x0)) {
            if (x0 > x1) {
                _drawLineLow(surface, x1, x0, y1, y0, color);
            } else {
                _drawLineLow(surface, x0, x1, y0, y1, color);
            }
        } else {
            if (y0 > y1) {
                _drawLineHigh(surface, x1, x0, y1, y0, color);
            } else {
                _drawLineHigh(surface, x0, x0, y0, y1, color);
            }
        }
    }

    class Renderer {
        public:
        SDL::SDL_Window* inner_window;
        SDL::SDL_Surface* inner_surface;

        Renderer(const char* title, int w, int h) {
            inner_window = SDL::SDL_CreateWindow(title, w, h, 0);
            if (inner_window == NULL) {
                fprintf(stderr, "[ERROR] Couldn't create window: %s\n", SDL::SDL_GetError());
                throw;
            }
            inner_surface = SDL::SDL_GetWindowSurface(inner_window);
            if (inner_surface == NULL) {
                fprintf(stderr, "[ERROR] Couldn't create surface: %s\n", SDL::SDL_GetError());
                throw;
            }
        }
        ~Renderer() {
            SDL::SDL_DestroyWindowSurface(inner_window);
        }
        void drawLine(int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
            _drawLine(inner_surface, x0, x1, y0, y1, color);
        }
    };
}
using namespace Render;

int main() {
    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 800, 600);

    SDL::SDL_Event event;

    render.drawLine(585, 15, 310, 310,  {255, 0, 0, 255});
    render.drawLine(15,  300, 50,  180, {0, 255, 0, 255});
    render.drawLine(585, 300, 50,  180, {0, 255, 0, 255});
    render.drawLine(15,  300, 310, 180, {0, 255, 0, 255});
    render.drawLine(585, 300, 310, 180, {0, 255, 0, 255});

    for (;;) {
        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
        }
        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
