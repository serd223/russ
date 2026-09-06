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
        std::vector<Face> faces;
        float scale = 1.0f;

        Model(const char* obj_file_path);
        Vec3 getRot() const;
        void setRot(Vec3 rot);
        void setRotX(float angle);
        void setRotY(float angle);
        void setRotZ(float angle);

        private:
        void recalculateNormals();
        Vec3 rot = {0, 0, 0};
        Mat3x3 transform = Mat3x3::id();
    };

    class Renderer {
        public:
        SDL::SDL_Window* inner_window;
        SDL::SDL_Surface* inner_surface;

        Renderer(const char* title, int w, int h);
        ~Renderer();
        void clear(Color color);
        void drawModel(Model& model, Color tint);
        void drawLine(int x0, int x1, int y0, int y1, Color color);
        void drawLine(Vec3 v1, Vec3 v2, Color color);
        void drawShape(std::span<const Vec3> vertices, std::span<const int> indices, Color color);
        void drawTriangleFilled(std::span<const iVec2, 3> vertex, Color color);
    };
}
