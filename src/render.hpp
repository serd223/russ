#pragma once

/*
    Camera and Renderer
    TODO : Refactor camera into a different file.
*/

#include <vector>
#include <rmath.hpp>
#include <span>
namespace SDL {
    #include <SDL3/SDL.h>
}
#include "model.hpp"
#include "camera.hpp"

typedef SDL::SDL_Color Color;

class Renderer {
    public:
    SDL::SDL_Window* inner_window;
    SDL::SDL_Surface* inner_surface;
    Camera cam;

    Renderer(const char* title, int w, int h);
    ~Renderer();
    void clear(Color color);
    void drawModel(Model& model, Vec3 position, Color tint);
    void drawLine(int x0, int x1, int y0, int y1, Color color);
    void drawLine(Vec3 v1, Vec3 v2, Color color);
    void drawShape(std::span<const Vec3> vertices, std::span<const int> indices, Color color);
    void drawTriangleFilled(std::span<const iVec2, 3> vertex, Color color);
};
