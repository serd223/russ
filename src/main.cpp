#include <cerrno>
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <rmath.hpp>
#include <render.hpp>
// #include "cube.cpp"

using namespace render;
using namespace rmath;

int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Usage: %s <path to .obj file>\n", argv[0]);
        return 1;
    }

    // Load models
    std::vector<Model> models;
    for (int i = 1; i < argc; i++) {
        const char* obj_file_path = argv[i];
        Model model(obj_file_path);
        model.scale = 70.0f;
        model.rot({M_PI, 0, 0});
        models.push_back(model);
    }

    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 800, 600);
    render.cam.rot(Vec3(0,0,0));

    bool isMouseLeftDown = false;
    Vec2 mouseRel;
    Vec3 position = {400, 300, -300};
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
                    printf("Frame Time: %f, FPS: %f\n", delta, 1.0 / delta);
                } else if (event.key.key == SDLK_LEFT) {
                    dir.x = 0;
                } else if (event.key.key == SDLK_RIGHT) {
                    dir.x = 0;
                } else if (event.key.key == SDLK_UP) {
                    dir.y = 0;
                } else if (event.key.key == SDLK_DOWN) {
                    dir.y = 0;
                }
            } else if (event.type == SDL::SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_LEFT) {
                    dir.x = -1;
                } else if (event.key.key == SDLK_RIGHT) {
                    dir.x = 1;
                } else if (event.key.key == SDLK_UP) {
                    dir.y = -1;
                } else if (event.key.key == SDLK_DOWN) {
                    dir.y = 1;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isMouseLeftDown = true;
                }
            } else if (event.type == SDL::SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
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
        position.x += 100.0f * delta * (float)dir.x;
        position.y += 100.0f * delta * (float)dir.y;

        if (isMouseLeftDown) {
            for (auto& model : models) {
                model.rot({
                    model.rot().x - (float)(M_PI * delta) * mouseRel.y,
                    model.rot().y - (float)(M_PI * delta) * mouseRel.x,
                    model.rot().z,
                });
            }
        } else {
            // render.cam.rot({
            //     render.cam.rot().x - (float)(M_PI * delta) * mouseRel.y,
            //     render.cam.rot().y - (float)(M_PI * delta) * mouseRel.x,
            //     render.cam.rot().z,
            // });
        }
        // model.setRot({
        //     model.getRot().x, //  + (float)(M_PI_4 * delta),
        //     model.getRot().y, //  + (float)(M_PI_4 * delta),
        //     model.getRot().z + (float)(M_PI_4 * delta),
        // });
        // model.setRotY(
        //     model.getRot().y + (float)(M_PI_4 * delta)
        // );
        render.clear({40, 44, 52, 255});
        for (size_t i = 0; i < models.size(); i++) {
            Vec3 offset = {(float)i * 100, (float)i * 100, 0};
            render.drawModel(models[i], position + offset, {255, 0, 0, 255});
        }
        // std::array<iVec2, 3> triangle = {iVec2(600, 200), iVec2(240, 370), iVec2(450, 570)};
        // render.drawTriangleFilled(triangle, {55, 156, 33, 255});

        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
