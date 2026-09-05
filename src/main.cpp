// #include <iostream>
namespace SDL {
    #include <SDL3/SDL.h>
}

int main() {
    SDL::SDL_Init(SDL_INIT_VIDEO);
    SDL::SDL_Window* win = SDL::SDL_CreateWindow("russ - dev", 800, 600, 0);

    SDL::SDL_Event event;
    for (;;) {
        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
        }
    }
    loop_end:

    SDL::SDL_DestroyWindow(win);
    return 0;
}
