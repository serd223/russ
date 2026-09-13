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
        float w = static_cast<float>(inner_surface->w), h = static_cast<float>(inner_surface->h);
        drawTriangleFilled( // - on the y because actual y coordinates are flipped
            Vec3((v1.x / v1.z * d) + w / 2.0, -(v1.y / v1.z * d) + h / 2.0, v1.z),
            Vec3((v2.x / v2.z * d) + w / 2.0, -(v2.y / v2.z * d) + h / 2.0, v2.z),
            Vec3((v3.x / v3.z * d) + w / 2.0, -(v3.y / v3.z * d) + h / 2.0, v3.z),
            finalColor, w, h
        );
    }
}

void Renderer::drawTriangleFilled(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2, Color color, float w, float h) {
    // Math explanation at:
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle//barycentric-coordinates.html

    Vec3 vertices[3] = {vertex0, vertex1, vertex2};
    if (vertices[1].y < vertices[0].y) std::swap(vertices[1], vertices[0]);
    if (vertices[2].y < vertices[0].y) std::swap(vertices[2], vertices[0]);
    if (vertices[2].y < vertices[1].y) std::swap(vertices[2], vertices[1]); // v2y > v1y > v0y
    int minx = std::max(std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x), 0.0f);
    int maxx = std::min(std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x), w);
    int miny = std::max(vertices[0].y, 0.0f);
    int maxy = std::min(vertices[2].y, h);
    Vec3 v0 = vertices[0];
    Vec3 v1 = vertices[1];
    Vec3 v2 = vertices[2];
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 N = v0v1.cross(v0v2);
    if (std::abs(N.z) < 1e-9) return;
    float inverse_nz = 1.0 / N.z;

    Vec3 v1v2 = v2 - v1;
    Vec3 v2v0 = v0 - v2;

    SDL::Uint32 pixel = SDL::SDL_MapSurfaceRGB(inner_surface, color.r, color.g, color.b);
    for (int y = miny; y <= maxy; y++) {
        SDL::Uint32* row = reinterpret_cast<SDL::Uint32*>(reinterpret_cast<unsigned char*>(inner_surface->pixels) + (y * inner_surface->pitch));
        for (int x = minx; x <= maxx; x++) {
            Vec3 p = {
                .x = static_cast<float>(x),
                .y = static_cast<float>(y),
                .z = 0.0,
            };
            // solve N * (p - v0) = 0
            p.z = v0.z - (N.x * (p.x - v0.x) + N.y * (p.y - v0.y)) * inverse_nz;
            Vec3 v1p = p - v1;
            Vec3 C = v1v2.cross(v1p);
            if (C * N  < 0) continue;
            Vec3 v2p = p - v2;
            C = v2v0.cross(v2p);
            if (C * N  < 0) continue;
            Vec3 v0p = p - v0;
            C = v0v1.cross(v0p);
            if (C * N  < 0) continue;

            if (p.z < z_at(x, y)) {
            // if (x >= 0 && x < inner_surface->w && y >= 0 && y < inner_surface->h && p.z < z_at(x, y)) {
                z_set(x, y, p.z);
                row[x] = pixel;
            }

        }
    }
}

