#pragma once

/*
    Main Renderer Interface
    - Handles SDL window creation and manages drawable surface
*/

#include <rmath.hpp>
#include <span>
namespace SDL {
    #include <SDL3/SDL.h>
}
#include <camera.hpp>

typedef SDL::SDL_Color Color;

class Face {
    public:
    iVec3 indices;
    Vec3 normal;
};

class Model {
    public:
    std::vector<Vec3> vertices;
    std::vector<Face> faces;
    float scale = 1.0f;
    Vec3 pos;
    Color color;

    Model(const char* obj_file_path);
    Model(std::span<const Vec3> vertices, std::span<const Face> faces, float scale, Vec3 pos);

    Vec3 rot() const;
    /// recalculates all normals
    void rot(Vec3 rot);
    /// reacalculates all normals
    void rotX(float angle);
    /// reacalculates all normals
    void rotY(float angle);
    /// reacalculates all normals
    void rotZ(float angle);
    void recalculateNormals();

    private:
    Vec3 m_rot = {0, 0, 0};
    Mat3x3 m_transform = Mat3x3::id();
};

struct Point {
    int x;
    int y;
    float z;
};

class Renderer {
    public:
    SDL::SDL_Window* inner_window;
    SDL::SDL_Surface* inner_surface;
    Camera cam = Camera(Vec3(0, 0, 0));

    Renderer(const char* title, int w, int h);
    ~Renderer();
    void clear(Color color);
    void drawModel(Model& model, Color tint, bool doLighting = true);
    void drawTriangleFilled(std::span<const Point, 3> vertex, Color color);
    float z_at(int x, int y);
    float z_set(int x, int y, float z);
    private:
    std::vector<float> m_z_buffer;
    size_t m_z_buffer_stride;
};
