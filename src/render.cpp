#include <errno.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include <span>
#include <stdio.h>
#include <vector>

namespace SDL {
    #include <SDL3/SDL.h>
}

#include <rmath.hpp>
#include <render.hpp>

typedef SDL::SDL_Color Color;

Model::Model(const char* obj_file_path) {
    FILE* f = fopen(obj_file_path, "r");
    if (f == NULL) {
        printf("[ERROR] Couldn't open file '%s': %s\n", obj_file_path, strerror(errno));
        throw;
    }

    printf("Loading object from file '%s'...\n", obj_file_path);

    float x, y, z;
    while (fscanf(f, "v %f %f %f\n", &x, &y, &z) >= 3) {
        vertices.push_back({x, y, z});
    };

    int a, b, c;
    while (fscanf(f, "f %d %d %d\n", &a, &b, &c) >= 3) {
        Vec3 v1 = vertices[a-1], v2 = vertices[b-1], v3 = vertices[c-1];
        Vec3 l1 = v2 - v1, l2 = v3 - v1;
        faces.push_back({.indices = {a - 1, b - 1, c - 1}, .normal = l1.cross(l2).normalize()});
    };

    printf("Succesfully loaded %lu vertices and %lu faces from file '%s'.\n", vertices.size(), faces.size(), obj_file_path);
}

Model::Model(std::span<const Vec3> vertices, std::span<const Face> faces, float scale, Vec3 pos) {
    this->vertices = std::vector(vertices.begin(), vertices.end());
    this->faces = std::vector(faces.begin(), faces.end());
    this->scale = scale;
    this->pos = pos;
    m_rot = {0,0,0};
    recalculateNormals();
}

void Model::recalculateNormals() {
    for (size_t i = 0; i < faces.size(); i++) {
        Vec3 v1 = vertices[faces[i].indices.a];
        Vec3 v2 = vertices[faces[i].indices.b];
        Vec3 v3 = vertices[faces[i].indices.c];
        Vec3 l1 = v2 - v1, l2 = v3 - v1;
        faces[i].normal = Mat3x3::rotXYZ(m_rot) * l1.cross(l2).normalize();
    }
}

Vec3 Model::rot() const {
    return this->m_rot;
}

void Model::rot(Vec3 rot) {
    this->m_rot = rot;
    recalculateNormals();
}

void Model::rotX(float angle) {
    this->m_rot.x = angle;
    recalculateNormals();
}

void Model::rotY(float angle) {
    this->m_rot.y = angle;
    recalculateNormals();
}

void Model::rotZ(float angle) {
    this->m_rot.z = angle;
    recalculateNormals();
}static void _writePixel(Renderer* render, int x, int y, float z_depth, Color color) {
    if (x >= 0 && x < render->inner_surface->w && y >= 0 && y < render->inner_surface->h && z_depth < render->z_at(x, y)) {
        render->z_set(x, y, z_depth);
        SDL::SDL_WriteSurfacePixel(render->inner_surface, x, y, color.r, color.g, color.b, color.a);
    }
}

static void _drawLineHigh(Renderer* render, int x0, int x1, int y0, int y1, float z0, float z1, Color color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) {
        float t = ((float)(y - y0))/((float)(y1 - y0));
        _writePixel(render, x, y, z0 + (z1 - z0) * t, color);
        if (D > 0) {
            x += xi;
            D += (2 * (dx - dy));
        } else {
            D += 2 * dx;
        }
    }
}

static void _drawLineLow(Renderer* render, int x0, int x1, int y0, int y1, float z0, float z1, Color color) {

    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int y = y0;

    // x - x0 / x1 - x0
    for (int x = x0; x <= x1; x++) {
        float t = ((float)(x - x0))/((float)(x1 - x0));
        _writePixel(render, x, y, z0 + (z1 - z0) * t, color);
        if (D > 0) {
            y += yi;
            D += (2 * (dy - dx));
        } else {
            D += 2 * dy;
        }
    }
}

static void _drawLine(Renderer* render, int x0, int x1, int y0, int y1, float z0, float z1, Color color) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            _drawLineLow(render, x1, x0, y1, y0, z1, z0, color);
        } else {
            _drawLineLow(render, x0, x1, y0, y1, z0, z1, color);
        }
    } else {
        if (y0 > y1) {
            _drawLineHigh(render, x1, x0, y1, y0, z1, z0, color);
        } else {
            _drawLineHigh(render, x0, x1, y0, y1, z0, z1, color);
        }
    }
}

Renderer::Renderer(const char* title, int w, int h) {
    inner_window = SDL::SDL_CreateWindow(title, w, h, 0);
    if (inner_window == NULL) {
        fprintf(stderr, "[ERROR] Couldn't create window: %s\n", SDL::SDL_GetError());
        throw;
    }
    inner_surface = SDL::SDL_GetWindowSurface(inner_window);
    if (inner_surface == NULL) {
        fprintf(stderr, "[ERROR] Couldn't create surface: %s\n", SDL::SDL_GetError());
        throw;
    }
    m_z_buffer.resize(w * h);
    for (size_t i = 0; i < (size_t)(w * h); i++) {
        m_z_buffer[i] = MAXFLOAT;
    }
    m_z_buffer_stride = w;
}

Renderer::~Renderer() {
        SDL::SDL_DestroyWindow(inner_window);
}
float Renderer::z_at(int x, int y) {
    return m_z_buffer[y * m_z_buffer_stride + x];
}

float Renderer::z_set(int x, int y, float z) {
    return m_z_buffer[y * m_z_buffer_stride + x] = z;
}
void Renderer::clear(Color color) {
    // TODO: error checking
    SDL::SDL_ClearSurface(inner_surface, (float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f);
    for (auto& f : m_z_buffer) {
        f = MAXFLOAT;
    }
}

void Renderer::drawModel(Model& model, Color tint, bool doLighting) {
    Vec3 position = model.pos - cam.pos;
    Mat3x3 rot = Mat3x3::rotXYZ(model.rot());

    // Don't move to camera space, just compute world coords relative to camera
    thread_local std::vector<Vec3> vertices_world; // leak
    // Take each vertex and move it to camera space
    thread_local std::vector<Vec3> vertices; // leak
    vertices_world.reserve(model.vertices.size());
    vertices.reserve(model.vertices.size());
    for (size_t i = 0; i < model.vertices.size();i++) {
        // Scale and offset vertices to world positions
        vertices_world[i] = ((rot * model.vertices[i]) * model.scale) + position;
        vertices[i] = vertices_world[i];
        vertices[i] = {vertices[i] * cam.right(), vertices[i] * cam.up(), vertices[i] * cam.front()};
    }

    // Face culling
    thread_local std::vector<Face> faces; // leak
    faces.clear();
    for (auto& face : model.faces) {
        Vec3 v1 = vertices_world[face.indices.a];
        Vec3 v2 = vertices_world[face.indices.b];
        Vec3 v3 = vertices_world[face.indices.c];

        // Back-face culling
        // Check if the angle between the ray from the camera to the middle of the ray and the face normal are pointing in different directions
        float dot = face.normal * (v1 + v2 + v3);
        if (dot >= 0.0f) {
            continue;
        };

        v1 = vertices[face.indices.a];
        v2 = vertices[face.indices.b];
        v3 = vertices[face.indices.c];
        std::vector<Vec3> vecs = {v1, v2, v3};
        // View Frustum culling
        if (!cam.draw(vecs)) continue;
        faces.push_back(face);
    }

    for (std::size_t i = 0; i < faces.size(); i++) {
        Vec3 n = faces[i].normal;

        Vec3 v1 = vertices[faces[i].indices.a];
        Vec3 v2 = vertices[faces[i].indices.b];
        Vec3 v3 = vertices[faces[i].indices.c];

        (void)tint;
        Color finalColor = model.color;
        if (doLighting) {
            // Fake lighting
            const float nMax = 0.65;
            if (n.y >= 0.0) {
                float t = n.y > nMax ? nMax : n.y;
                finalColor.r = (int)((float)finalColor.r * (1.0 - t));
                finalColor.g = (int)((float)finalColor.g * (1.0 - t));
                finalColor.b = (int)((float)finalColor.b * (1.0 - t));
            }
            if (n.x >= 0.0) {
                float t = n.x > nMax ? nMax : n.x;
                finalColor.r = (int)((float)finalColor.r * (1.0 - t));
                finalColor.g = (int)((float)finalColor.g * (1.0 - t));
                finalColor.b = (int)((float)finalColor.b * (1.0 - t));
            }
        }

        const float d = 1000.0;
        drawTriangleFilled( // - on the y because actual y coordinates are flipped
            {(int)(v1.x / v1.z * d) + inner_surface->w / 2, -(int)(v1.y / v1.z * d) + inner_surface->h / 2, v1.z},
            {(int)(v2.x / v2.z * d) + inner_surface->w / 2, -(int)(v2.y / v2.z * d) + inner_surface->h / 2, v2.z},
            {(int)(v3.x / v3.z * d) + inner_surface->w / 2, -(int)(v3.y / v3.z * d) + inner_surface->h / 2, v3.z},
            finalColor
        );
    }
}

void Renderer::drawTriangleFilled(const Point& vertex0, const Point& vertex1, const Point& vertex2, Color color) {
    Point vertices[3] = {vertex0, vertex1, vertex2};
    if (vertices[1].y < vertices[0].y) std::swap(vertices[1], vertices[0]);
    if (vertices[2].y < vertices[0].y) std::swap(vertices[2], vertices[0]);
    if (vertices[2].y < vertices[1].y) std::swap(vertices[2], vertices[1]); // v2y > v1y > v0y

    float l02_inverse_y_dist = 1.0 / (float)(vertices[2].y - vertices[0].y);
    float l02_x_t = (float)(vertices[2].x - vertices[0].x) * l02_inverse_y_dist;
    float l02_z_t = (float)(vertices[2].z - vertices[0].z) * l02_inverse_y_dist;

    float l01_inverse_y_dist = 1.0 / (float)(vertices[1].y - vertices[0].y);
    float l01_x_t = (float)(vertices[1].x - vertices[0].x) * l01_inverse_y_dist;
    float l01_z_t = (float)(vertices[1].z - vertices[0].z) * l01_inverse_y_dist;

    float l12_inverse_y_dist = 1.0 / (float)(vertices[2].y - vertices[1].y);
    float l12_x_t = (float)(vertices[2].x - vertices[1].x) * l12_inverse_y_dist;
    float l12_z_t = (float)(vertices[2].z - vertices[1].z) * l12_inverse_y_dist;

    for (int y = vertices[0].y; y < vertices[1].y; y++) {
        int l02x = static_cast<int>((y - vertices[0].y) * l02_x_t + vertices[0].x);
        float l02z = (y - vertices[0].y) * l02_z_t + vertices[0].z;
        int l01x = static_cast<int>((y - vertices[0].y) * l01_x_t + vertices[0].x);
        float l01z = (y - vertices[0].y) * l01_z_t + vertices[0].z;
        _drawLine(this, l02x, l01x, y, y, l02z, l01z, color);
    }
    for (int y = vertices[1].y; y < vertices[2].y; y++) {
        int l02x = static_cast<int>((y - vertices[0].y) * l02_x_t + vertices[0].x);
        float l02z = (y - vertices[0].y) * l02_z_t + vertices[0].z;
        int l12x = static_cast<int>((y - vertices[1].y) * l12_x_t + vertices[1].x);
        float l12z = (y - vertices[1].y) * l12_z_t + vertices[1].z;
        _drawLine(this, l02x, l12x, y, y, l02z, l12z, color);
    }
}

