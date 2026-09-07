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
        Vec3 rot() const;
        void rot(Vec3 rot);
        void rotX(float angle);
        void rotY(float angle);
        void rotZ(float angle);

        private:
        void recalculateNormals();
        Vec3 m_rot = {0, 0, 0};
        Mat3x3 m_transform = Mat3x3::id();
    };

    class Camera {
        public:
        Vec3 rot() const;
        void rot(Vec3 newRot);
        Vec3 front() const;
        Vec3 up() const;
        Vec3 right() const;

        private:
        Vec3 m_rot;
        Vec3 m_up;
        Vec3 m_right;
        Vec3 m_front;
    };
    
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
}
