#include <math.h>
#include <stdio.h>
#include <vector>

namespace SDL {
    #include <SDL3/SDL.h>
}

namespace Render {
    typedef struct {
        float x, y, z;
        SDL::SDL_Color color;
    } Vec3;

    static void _drawLineHigh(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
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

    static void _drawLineLow(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {

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

    static void _drawLine(SDL::SDL_Surface* surface, int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
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

    class Renderer {
        public:
        SDL::SDL_Window* inner_window;
        SDL::SDL_Surface* inner_surface;

        Renderer(const char* title, int w, int h) {
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
        ~Renderer() {
            SDL::SDL_DestroyWindowSurface(inner_window);
        }

        void clear(SDL::SDL_Color color) {
            // TODO: error checking
            SDL::SDL_ClearSurface(inner_surface, (float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f);
        }
        
        void drawLine(int x0, int x1, int y0, int y1, SDL::SDL_Color color) {
            _drawLine(inner_surface, x0, x1, y0, y1, color);
        }

        Vec3 transformVertexRotZ(Vec3 vec, float tetha) {
            return {
                (vec.x * cosf(tetha) - vec.y * sinf(tetha)),
                (vec.x * sinf(tetha) + vec.y * cosf(tetha)),
                vec.z,
                vec.color,
            };
        }

        Vec3 transformVertexRotY(Vec3 vec, float tetha) {
            return {
                (vec.x * cosf(tetha) + vec.z * sinf(tetha)),
                vec.y,
                -(vec.x * sinf(tetha) + vec.z * cosf(tetha)),
                vec.color,
            };
        }

        Vec3 transformVertexRotX(Vec3 vec, float tetha) {
            return {
                vec.x,
                (vec.y * cosf(tetha) - vec.z * sinf(tetha)),
                (vec.y * sinf(tetha) + vec.z * cosf(tetha)),
                vec.color,
            };
        }

        void drawShape(std::vector<Vec3> vertices, std::vector<int> indices, float angle) {
            for (std::size_t i = 0; i < indices.size() - 1; i++) {
                Vec3 vec1 = transformVertexRotX(vertices[indices[i]], angle);
                vec1 = transformVertexRotY(vec1, angle);
                vec1 = transformVertexRotZ(vec1, angle);
                Vec3 vec2 = transformVertexRotX(vertices[indices[i + 1]], angle);
                vec2 = transformVertexRotY(vec2, angle);
                vec2 = transformVertexRotZ(vec2, angle);
                drawLine(
                         (int)(vec1.x * 100.0f + 200.0f),
                         (int)(vec2.x * 100.0f + 200.0f),
                         (int)(vec1.y * 100.0f + 200.0f),
                         (int)(vec2.y * 100.0f + 200.0f),
                         vec1.color
                 );
            }
        }
    };
}
using namespace Render;

int main() {
    SDL::SDL_Init(SDL_INIT_VIDEO);
    Renderer render = Renderer("russ - dev", 800, 600);

    std::vector<Vec3> vertices = {
        /*0*/{ 0.5f,   0.5f,   0.5f, {255, 0, 0, 255}},
        /*1*/{ 0.5f,   0.5f,  -0.5f, {255, 0, 0, 255}},
        /*2*/{-0.5f,   0.5f,  -0.5f, {255, 0, 0, 255}},
        /*3*/{-0.5f,   0.5f,   0.5f, {255, 0, 0, 255}},
        /*4*/{-0.5f,  -0.5f,   0.5f, {255, 0, 0, 255}},
        /*5*/{ 0.5f,  -0.5f,   0.5f, {255, 0, 0, 255}},
        /*6*/{ 0.5f,  -0.5f,  -0.5f, {255, 0, 0, 255}},
        /*7*/{-0.5f,  -0.5f,  -0.5f, {255, 0, 0, 255}},
    };
    auto indexArr = {
        0, 1, 2 ,3, 0, 5, 6, 5, 4, 7, 6, 1, 2, 7, 4, 3
    };

    SDL::Uint32 now = SDL::SDL_GetPerformanceCounter();
    SDL::Uint32 last;
    double delta;
    float angle = 0.0f;

    SDL::SDL_Event event;
    for (;;) {
        last = now;
        now = SDL::SDL_GetPerformanceCounter();
        delta = (double)(now - last) / (double)(SDL::SDL_GetPerformanceFrequency());

        while (SDL::SDL_PollEvent(&event)) {
            if (event.type == SDL::SDL_EVENT_QUIT) goto loop_end;
        }
        angle += M_PI_4 * delta;
        render.clear({40, 44, 52, 255});
        render.drawShape(vertices, indexArr, angle);
        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
