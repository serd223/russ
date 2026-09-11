#include <camera.hpp>
#include <math.h>
#include <stdio.h>
#include <rmath.hpp>
#include <render.hpp> // Includes camera.hpp and model.hpp

typedef enum {
    KEY_UP = 'Z' + 1,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEYS_COUNT
} RussKey;

int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Usage: %s <path to .obj file>\n", argv[0]);
        return 1;
    }

    Vec3 position = {0, 0, -300};

    // Load models
    std::vector<Model> models;
    for (int i = 1; i < argc; i++) {
        const char* obj_file_path = argv[i];
        Model model(obj_file_path);
        model.scale = 15.0f;
        model.pos = position;
        if (i == 1) {
            model.color = {255, 0, 0, 255};
        } else {
            model.color = {0, 255, 0, 255};
        }
        models.push_back(model);
    }
    {
        const Vec3 vs[] = {
            {-5.0f,-5.0f,-55.0f},
            { 5.0f,-5.0f,-55.0f},
            {-5.0f,-5.0f,-65.0f},
            { 5.0f,-5.0f,-65.0f}
        };
        const Face fs[] = {
            {{0, 1, 2}, {}},
            {{1, 3, 2}, {}}
        };
        Model floor_model(vs, fs, 5.0f, {0.0f, 0.0f, 0.0f});
        floor_model.color = {0, 0, 255, 255};
        models.push_back(floor_model);
    }
    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 1200, 900);
    bool mouse_hide = false, mouse_hide_override = true;
    SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, mouse_hide); // grab and hide mouse cursor
    render.cam.pos.y = 80.;
    render.cam.rot(Vec3(
        -M_PI_4 / 3.0,
        render.cam.rot().y,
        render.cam.rot().z 
    ));
    bool isMouseLeftDown = false;
    Vec2 mouseRel;

    SDL::Uint32 now = SDL::SDL_GetPerformanceCounter();
    SDL::Uint32 last;
    double delta;

    std::vector<bool> keys;
    keys.resize(KEYS_COUNT);
    SDL::SDL_Event event;
    for (;;) {
        last = now;
        now = SDL::SDL_GetPerformanceCounter();
        delta = (double)(now - last) / (double)(SDL::SDL_GetPerformanceFrequency());
        (void)delta;

        mouseRel.x = 0.0f, mouseRel.y = 0.0f;
        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
            else if (event.type == SDL::SDL_EVENT_KEY_UP) {
                if (SDLK_A <= event.key.key && event.key.key <= SDLK_Z) {
                    keys[event.key.key - SDLK_A + 'A'] = false;
                }
                switch (event.key.key) {
                case SDLK_UP: keys[KEY_UP] = false; break;
                case SDLK_DOWN: keys[KEY_DOWN] = false; break;
                case SDLK_LEFT: keys[KEY_LEFT] = false; break;
                case SDLK_RIGHT: keys[KEY_RIGHT] = false; break;
                case SDLK_SPACE: {
                    printf("Frame Time: %.3fms, FPS: %.3f\n", delta * 1000.0, 1.0 / delta);
                } break;
                default: break;
                }
            } else if (event.type == SDL::SDL_EVENT_KEY_DOWN) {
                if (SDLK_A <= event.key.key && event.key.key <= SDLK_Z) {
                    keys[event.key.key - SDLK_A + 'A'] = true;
                }
                switch (event.key.key) {
                case SDLK_UP: keys[KEY_UP] = true; break;
                case SDLK_DOWN: keys[KEY_DOWN] = true; break;
                case SDLK_LEFT: keys[KEY_LEFT] = true; break;
                case SDLK_RIGHT: keys[KEY_RIGHT] = true; break;
                case SDLK_ESCAPE: {
                    goto loop_end;
                } break;
                case SDLK_G: {
                    mouse_hide = !mouse_hide;
                    mouse_hide_override = !mouse_hide_override;
                    SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, mouse_hide);
                } break;
                default: break;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (!mouse_hide_override) {
                        mouse_hide = false;
                        SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, mouse_hide);
                    }
                    isMouseLeftDown = true;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (!mouse_hide_override) {
                        mouse_hide = true;
                        SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, mouse_hide);
                    }
                    isMouseLeftDown = false;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_MOTION) {
                mouseRel.x = event.motion.xrel;
                mouseRel.y = event.motion.yrel;
            } else if (event.type == SDL::SDL_EVENT_MOUSE_WHEEL) {
                models[0].scale += event.wheel.y * delta * 100.0f;
            }
        }

        if (isMouseLeftDown) {
            models[0].rot({
                models[0].rot().x - (float)(M_PI * delta) * mouseRel.y,
                models[0].rot().y + (float)(M_PI * delta) * mouseRel.x,
                models[0].rot().z,
            });
        } else if (mouse_hide) {
            Vec3 newCamRot = {
                render.cam.rot().x - (float)(M_PI_4 * delta) * mouseRel.y,
                render.cam.rot().y + (float)(M_PI_4 * delta) * mouseRel.x,
                render.cam.rot().z,
            };
            if (newCamRot.x >=  M_PI_2 - 0.001) newCamRot.x = M_PI_2 - 0.001;
            if (newCamRot.x <= -M_PI_2 + 0.001) newCamRot.x = -M_PI_2 + 0.001;

            render.cam.rot(newCamRot);
        }

        {
            int df = 0;
            int dr = 0;
            if (keys['W']) df -= 1;
            if (keys['A']) dr -= 1;
            if (keys['S']) df += 1;
            if (keys['D']) dr += 1;
            render.cam.pos = render.cam.pos - 60.0 * delta * (float)df * render.cam.front();
            render.cam.pos = render.cam.pos + 60.0 * delta * (float)dr * render.cam.right();
        }
        {
            int df = 0;
            int dr = 0;
            if (keys[KEY_UP]) df -= 1;
            if (keys[KEY_LEFT]) dr -= 1;
            if (keys[KEY_DOWN]) df += 1;
            if (keys[KEY_RIGHT]) dr += 1;
            models[0].pos = models[0].pos - 60.0 * delta * (float)df * render.cam.front();
            models[0].pos = models[0].pos + 60.0 * delta * (float)dr * render.cam.right();
        }
        render.clear({40, 44, 52, 255});
        for (size_t i = 0; i < models.size(); i++) {
            render.drawModel(models[i], {255, 0, 0, 255});
        }

        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
