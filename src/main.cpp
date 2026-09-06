#include <cerrno>
#include <cstring>
#include <math.h>
#include <span>
#include <stdio.h>
#include <vector>

namespace SDL {
    #include <SDL3/SDL.h>
}

#include <rmath.hpp>
#include <render.hpp>
#include "cube.hpp"

using namespace render;
using namespace rmath;

int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Usage: %s <path to .obj file>\n", argv[0]);
        return 1;
    }

    const char* obj_file_path = argv[1];

    Model model(obj_file_path);
    model.scale = 50.0f;
    model.rot = {M_PI, 0, 0};

    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 800, 600);

    SDL::Uint32 now = SDL::SDL_GetPerformanceCounter();
    SDL::Uint32 last;
    double delta;

    SDL::SDL_Event event;
    for (;;) {
        last = now;
        now = SDL::SDL_GetPerformanceCounter();
        delta = (double)(now - last) / (double)(SDL::SDL_GetPerformanceFrequency());

        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
        }
        model.rot.y += M_PI_4 * delta;
        render.clear({40, 44, 52, 255});
        render.drawModel(model, {255, 0, 0, 255});

        std::array<iVec2, 3> triangle = {iVec2(600, 200), iVec2(240, 370), iVec2(450, 570)};
        std::array<int, 3> indexes = {0, 1, 2};
        render.drawTriangleFilled(triangle, indexes, {55, 156, 33, 255});

        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
