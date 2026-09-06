#pragma once

#include <vector>
#include <rmath.hpp>
#include <span>
namespace SDL {
    #include <SDL3/SDL.h>
}

using namespace rmath;

namespace render {
    typedef SDL::SDL_Color Color;

    class Model {
        public:
        std::vector<Vec3> vertices;
        std::vector<iVec3> faces;
        Vec3 rot = {0, 0, 0};
        float scale = 1.0f;
        Mat3x3 transform = Mat3x3::id();

        Model(std::vector<Vec3> vertices, std::vector<iVec3> faces);
        Model(const char* obj_file_path);

    };

    static void _drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color);
    static void _drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color);
    static void _drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color);

    class Renderer {
        public:
        SDL::SDL_Window* inner_window;
        SDL::SDL_Surface* inner_surface;

        Renderer(const char* title, int w, int h);
        ~Renderer();
        void clear(Color color);
        void drawLine(int x0, int x1, int y0, int y1, Color color);
        
        void drawLine(Vec3 v1, Vec3 v2, Color color);

        void drawModel(Model& model, Color tint);
        
        void drawShape(std::span<const Vec3> vertices, std::span<const int> indices, Color color);
    };
}