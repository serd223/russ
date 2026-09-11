#include "camera.hpp"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <rmath.hpp>
#include <render.hpp> // Includes camera.hpp and model.hpp

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

        model.recalculateNormals(); // because vertices are rotated
        models.push_back(model);
    }
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

    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 1200, 900);
    SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, true); // grab and hide mouse cursor
    render.cam.pos.y = 80.;
    render.cam.rot(Vec3(
        -M_PI_4 / 3.0,
        render.cam.rot().y,
        render.cam.rot().z 
    ));
    bool isMouseLeftDown = false;
    Vec2 mouseRel;
    iVec2 dir = {0, 0};

    SDL::Uint32 now = SDL::SDL_GetPerformanceCounter();
    SDL::Uint32 last;
    double delta;

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
                if (event.key.key == SDLK_SPACE) {
                    printf("Frame Time: %.3fms, FPS: %.3f\n", delta * 1000.0, 1.0 / delta);
                } else if (event.key.key == SDLK_A) {
                    dir.x = 0;
                } else if (event.key.key == SDLK_D) {
                    dir.x = 0;
                } else if (event.key.key == SDLK_W) {
                    dir.y = 0;
                } else if (event.key.key == SDLK_S) {
                    dir.y = 0;
                } 
            } else if (event.type == SDL::SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    goto loop_end;
                } else if (event.key.key == SDLK_A) {
                    dir.x = -1;
                } else if (event.key.key == SDLK_D) {
                    dir.x = 1;
                } else if (event.key.key == SDLK_W) {
                    dir.y = -1;
                } else if (event.key.key == SDLK_S) {
                    dir.y = 1;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, false);
                    isMouseLeftDown = true;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    SDL::SDL_SetWindowRelativeMouseMode(render.inner_window, true);
                    isMouseLeftDown = false;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_MOTION) {
                mouseRel.x = event.motion.xrel;
                mouseRel.y = event.motion.yrel;
            } else if (event.type == SDL::SDL_EVENT_MOUSE_WHEEL) {
                for (auto& model : models) {
                    model.scale += event.wheel.y * delta * 100.0f;
                }
            }
        }

        if (isMouseLeftDown) {
            for (auto& model : models) {
                model.rot({
                    model.rot().x - (float)(M_PI * delta) * mouseRel.y,
                    model.rot().y - (float)(M_PI * delta) * mouseRel.x,
                    model.rot().z,
                });
            }
        } else {
            Vec3 newCamRot = {
                render.cam.rot().x - (float)(M_PI_4 * delta) * mouseRel.y,
                render.cam.rot().y + (float)(M_PI_4 * delta) * mouseRel.x,
                render.cam.rot().z,
            };
            if (newCamRot.x >=  M_PI_2 - 0.001) newCamRot.x = M_PI_2 - 0.001;
            if (newCamRot.x <= -M_PI_2 + 0.001) newCamRot.x = -M_PI_2 + 0.001;

            render.cam.rot(newCamRot);
        }

        render.cam.pos = render.cam.pos - 60.0 * delta * (float)dir.y * render.cam.front();
        render.cam.pos = render.cam.pos + 60.0 * delta * (float)dir.x * render.cam.right();

        render.clear({40, 44, 52, 255});
        render.drawModel(floor_model, {0,0,128,255}, false);
        for (size_t i = 0; i < models.size(); i++) {
            render.drawModel(models[i], {255, 0, 0, 255});
        }

        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
