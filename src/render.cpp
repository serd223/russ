#include <math.h>
#include <cerrno>
#include <cstring>
#include <span>
#include <stdio.h>
#include <vector>


namespace SDL {
    #include <SDL3/SDL.h>
}

#include "render.hpp"

namespace render {
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
                faces.push_back({a, b, c});
            };
            if (fscanf(f, "f %d %d %d", &a, &b, &c) >= 3) {
                faces.push_back({a, b, c});
            }

            printf("Succesfully loaded %lu vertices and %lu faces from file '%s'.\n", vertices.size(), faces.size(), obj_file_path);
        }

    Model::Model(std::vector<Vec3> vertices, std::vector<iVec3> faces) {
            this->vertices = vertices;
            this->faces = faces;
        }

    void _drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
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
            SDL::SDL_WriteSurfacePixel(surface, x, y, color.r, color.g, color.b, color.a);
            if (D > 0) {
                x += xi;
                D += (2 * (dx - dy));
            } else {
                D += 2 * dx;
            }
        }
    }

    void _drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {

        int dx = x1 - x0;
        int dy = y1 - y0;
        int yi = 1;
        if (dy < 0) {
            yi = -1;
            dy = -dy;
        }
        int D = (2 * dy) - dx;
        int y = y0;

        for (int x = x0; x <= x1; x++) {
            SDL::SDL_WriteSurfacePixel(surface, x, y, color.r, color.g, color.b, color.a);
            if (D > 0) {
                y += yi;
                D += (2 * (dy - dx));
            } else {
                D += 2 * dy;
            }
        }
    }

    void _drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
        if (abs(y1 - y0) < abs(x1 - x0)) {
            if (x0 > x1) {
                _drawLineLow(surface, x1, x0, y1, y0, color);
            } else {
                _drawLineLow(surface, x0, x1, y0, y1, color);
            }
        } else {
            if (y0 > y1) {
                _drawLineHigh(surface, x1, x0, y1, y0, color);
            } else {
                _drawLineHigh(surface, x0, x1, y0, y1, color);
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
    }
    
    Renderer::~Renderer() {
            SDL::SDL_DestroyWindow(inner_window);
    }

    void Renderer::clear(Color color) {
        // TODO: error checking
        SDL::SDL_ClearSurface(inner_surface, (float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f);
    }

    void Renderer::drawLine(int x0, int x1, int y0, int y1, Color color) {
        _drawLine(inner_surface, x0, x1, y0, y1, color);
    }

    void Renderer::drawLine(Vec3 v1, Vec3 v2, Color color) {
        drawLine((int)v1.x, (int)v2.x, (int)v1.y, (int)v2.y, color);
    }
    
    void Renderer::drawModel(Model& model, Color tint) {
        for (std::size_t i = 0; i < model.faces.size(); i++) {
            Vec3 v1 = model.transform * Mat3x3::rotXYZ(model.rot) * model.vertices[model.faces[i].a];
            Vec3 v2 = model.transform * Mat3x3::rotXYZ(model.rot) * model.vertices[model.faces[i].b];
            Vec3 v3 = model.transform * Mat3x3::rotXYZ(model.rot) * model.vertices[model.faces[i].c];
            Vec3 l1 = (v1 - v2);
            Vec3 l2 = (v1 - v3);
            Vec3 n = l1.cross(l2);

            if (n.z < 0.0) continue;

            // TODO: Have some way outside of this method to specify position
            const Vec3 position = {200, 200, 200};
            drawLine(v1 * model.scale + position, v2 * model.scale + position, tint);
            drawLine(v2 * model.scale + position, v3 * model.scale + position, tint);
            drawLine(v3 * model.scale + position, v1 * model.scale + position, tint);
        }
    }

    void Renderer::drawShape(std::span<const Vec3> vertices, std::span<const int> indices, Color color) {
        for (std::size_t i = 0; i < indices.size() - 1; i++) {
            Vec3 vec1 = vertices[indices[i]];
            Vec3 vec2 = vertices[indices[i + 1]];
            drawLine(
                        (int)(vec1.x * 50.0f + 200.0f),
                        (int)(vec2.x * 50.0f + 200.0f),
                        (int)(vec1.y * 50.0f + 200.0f),
                        (int)(vec2.y * 50.0f + 200.0f),
                        color
                );
        }
    }
}