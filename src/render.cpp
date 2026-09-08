#include "rmath.hpp"
#include <algorithm>
#include <math.h>
#include <span>
#include <stdio.h>
#include <vector>

namespace SDL {
    #include <SDL3/SDL.h>
}

#include <model.hpp>
#include <render.hpp>

typedef SDL::SDL_Color Color;

static void _writePixel(SDL::SDL_Surface* surface, int x, int y, Color color) {
    if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) {
        SDL::SDL_WriteSurfacePixel(surface, x, y, color.r, color.g, color.b, color.a);
    }
}

static void _drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, Color color) {
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
        _writePixel(surface, x, y, color);
        if (D > 0) {
            x += xi;
            D += (2 * (dx - dy));
        } else {
            D += 2 * dx;
        }
    }
}

static void _drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, Color color) {

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
        _writePixel(surface, x, y, color);
        if (D > 0) {
            y += yi;
            D += (2 * (dy - dx));
        } else {
            D += 2 * dy;
        }
    }
}

static void _drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, Color color) {
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

static std::vector<iVec2> _interpolate(iVec2& v0, iVec2& v1) {
    std::vector<iVec2> out;
    for (int i = v0.y; i < v1.y; i++) {
        float o = (float)((i - v0.y) * (v1.x - v0.x)) / (float)(v1.y - v0.y) + v0.x;
        int ox = static_cast<int>(round(o));
        iVec2 v{ox, i};
        out.push_back(v);
    }
    return out;
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
    Vec3 position = model.pos - cam.pos;
    // TODO: I think **some** piece of math here still implicitly expects
    // cam.front to be (0, 0, -1) because something is still wrong with camera rotation

    Mat3x3 rot = Mat3x3::rotXYZ(model.rot());

    static std::vector<Vec3> vertices; // leak
    vertices.resize(model.vertices.size());
    for (size_t i = 0; i < model.vertices.size(); i++) {
        // Scale and offset vertices to world positions
        vertices[i] = ((rot * model.vertices[i]) * model.scale) + position;
    }
    std::sort(model.faces.begin(), model.faces.end(), [this, model](const Face& a, const Face& b) {
        Vec3 v1a = vertices[a.indices.a];
        Vec3 v2a = vertices[a.indices.b];
        Vec3 v3a = vertices[a.indices.c];
        // We basically want to calculate the perpandicular distance from
        // the middle of the face to span(cam.right, cam.up). The correct
        // formula would be ((v1a + v2a + v3a)/3 * front)/|front| but since this is
        // purely for sorting we can drop /3 and the ordering will reamin
        // the same. And we can drop |front| because up,right,front are unit vectors.
        float la = (v1a + v2a + v3a) * cam.front();

        Vec3 v1b = vertices[b.indices.a];
        Vec3 v2b = vertices[b.indices.b];
        Vec3 v3b = vertices[b.indices.c];
        float lb = (v1b + v2b + v3b) * cam.front();
        return la > lb;
    });

    // for (std::size_t i = model.faces.size() - 1; i > 0; i--) {
    for (std::size_t i = 0; i < model.faces.size(); i++) {
        Vec3 n  = model.faces[i].normal; // Rotation already applied

        // n . front = |n|.|front|.cos(a), the sign of cos(a) tells us whether the
        // two vectors are pointing in the same direction
        if ((n * cam.front()) >= 0.0) continue;

        Vec3 v1 = vertices[model.faces[i].indices.a];
        Vec3 v2 = vertices[model.faces[i].indices.b];
        Vec3 v3 = vertices[model.faces[i].indices.c];

        Color finalColor = tint;
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

        Vec3 c1 = { v1 * cam.right(), v1 * cam.up(), v1 * cam.front() };
        Vec3 c2 = { v2 * cam.right(), v2 * cam.up(), v2 * cam.front() };
        Vec3 c3 = { v3 * cam.right(), v3 * cam.up(), v3 * cam.front() };

        const float d = 1000.0;
        if (c1.z < 0.15 || c2.z < 0.15 || c3.z < 0.15) continue;
        drawTriangleFilled((iVec2[]){ // - on the y because actual y coordinates are flipped
                {(int)(c1.x / c1.z * d) + inner_surface->w / 2, -(int)(c1.y / c1.z * d) + inner_surface->h / 2},
                {(int)(c2.x / c2.z * d) + inner_surface->w / 2, -(int)(c2.y / c2.z * d) + inner_surface->h / 2},
                {(int)(c3.x / c3.z * d) + inner_surface->w / 2, -(int)(c3.y / c3.z * d) + inner_surface->h / 2}
            },
            finalColor
        );
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

void Renderer::drawTriangleFilled(std::span<const iVec2, 3> vertex, Color color) {
    // Accept vertices as span view and copy them to internal buffer
    iVec2 vertices[3] = {vertex[0], vertex[1], vertex[2]};
    for (auto& v : vertices) {
        if (v.x < 0 || v.x > inner_surface->w || v.y < 0 || v.y > inner_surface->h) return;
    }
    if (vertices[1].y < vertices[0].y) std::swap(vertices[1], vertices[0]);
    if (vertices[2].y < vertices[0].y) std::swap(vertices[2], vertices[0]);
    if (vertices[2].y < vertices[1].y) std::swap(vertices[2], vertices[1]); // v2y > v1y > v0y

    std::vector<iVec2> l02 = _interpolate(vertices[0], vertices[2]);
    std::vector<iVec2> l01 = _interpolate(vertices[0], vertices[1]);
    std::vector<iVec2> l12 = _interpolate(vertices[1], vertices[2]);

    l01.insert(l01.end(), l12.begin(), l12.end());
    
    for (size_t i = 0; i < l02.size(); i++) {
        _drawLine(inner_surface, l02[i].x, l01[i].x, l02[i].y, l01[i].y, color);
    }
}

