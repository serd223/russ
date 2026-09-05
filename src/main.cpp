#include <math.h>
#include <stdio.h>
#include <vector>

namespace SDL {
    #include <SDL3/SDL.h>
}

namespace Render {
    typedef struct __Vec3 {
        float x, y, z;

        struct __Vec3 cross(struct __Vec3 rhs) {
            return {
                this->y * rhs.z - this->z * rhs.y,
                this->z * rhs.x - this->x * rhs.z,
                this->x * rhs.y - this->y * rhs.x
            };
        }
    } Vec3;

    Vec3 operator - (const Vec3& lhs, const Vec3& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z,
        };   
    }
    typedef struct {
        int a, b, c;
    } Face;

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
            };
        }

        Vec3 transformVertexRotY(Vec3 vec, float tetha) {
            return {
                (vec.x * cosf(tetha) + vec.z * sinf(tetha)),
                vec.y,
                -(vec.x * sinf(tetha) + vec.z * cosf(tetha)),
            };
        }

        Vec3 transformVertexRotX(Vec3 vec, float tetha) {
            return {
                vec.x,
                (vec.y * cosf(tetha) - vec.z * sinf(tetha)),
                (vec.y * sinf(tetha) + vec.z * cosf(tetha)),
            };
        }

        Vec3 transformVertexRot(Vec3 vec, float tetha) {
            Vec3 ret = vec;
            ret = transformVertexRotX(ret, M_PI);
            ret = transformVertexRotY(ret, tetha);
            // ret = transformVertexRotZ(ret, tetha);
            return ret;
            
        }

        void drawModel(std::vector<Vec3> vertices, std::vector<Face> faces, float angle) {
            for (std::size_t i = 0; i < faces.size(); i++) {
                Vec3 v1 = transformVertexRot(vertices[faces[i].a], angle);
                Vec3 v2 = transformVertexRot(vertices[faces[i].b], angle);
                Vec3 v3 = transformVertexRot(vertices[faces[i].c], angle);
                Vec3 l1 = (v1 - v2);
                Vec3 l2 = (v1 - v3);
                Vec3 n = l1.cross(l2);

                if (n.z < 0.0) continue;
                drawShape(vertices, {faces[i].a, faces[i].b, faces[i].c, faces[i].a}, angle, {255, 0, 0, 255});
            }
        }
        
        void drawShape(std::vector<Vec3> vertices, std::vector<int> indices, float angle, SDL::SDL_Color color) {
            for (std::size_t i = 0; i < indices.size() - 1; i++) {
                Vec3 vec1 = transformVertexRot(vertices[indices[i]], angle);
                Vec3 vec2 = transformVertexRot(vertices[indices[i + 1]], angle);
                drawLine(
                         (int)(vec1.x * 50.0f + 200.0f),
                         (int)(vec2.x * 50.0f + 200.0f),
                         (int)(vec1.y * 50.0f + 200.0f),
                         (int)(vec2.y * 50.0f + 200.0f),
                         color
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
        // /*0*/{ 0.5f,   0.5f,   0.5f},
        // /*1*/{ 0.5f,   0.5f,  -0.5f},
        // /*2*/{-0.5f,   0.5f,  -0.5f},
        // /*3*/{-0.5f,   0.5f,   0.5f},
        // /*4*/{-0.5f,  -0.5f,   0.5f},
        // /*5*/{ 0.5f,  -0.5f,   0.5f},
        // /*6*/{ 0.5f,  -0.5f,  -0.5f},
        // /*7*/{-0.5f,  -0.5f,  -0.5f},
    };

    std::vector<Face> faces = {
        // Top face (+Y)
        // {0, 1, 2},
        // {0, 2, 3},

        // // Bottom face (-Y)
        // {5, 7, 6},
        // {5, 4, 7},

        // // Front face (+Z)
        // {0, 3, 4},
        // {0, 4, 5},

        // // Back face (-Z)
        // {1, 6, 7},
        // {1, 7, 2},

        // // Right face (+X)
        // {0, 5, 6},
        // {0, 6, 1},

        // // Left face (-X)
        // {3, 2, 7},
        // {3, 7, 4},
    };
    FILE* f = fopen("./obj/teapot.obj", "r");
    float x, y, z;
    while (fscanf(f, "v %f %f %f\n", &x, &y, &z) >= 3) {
        vertices.push_back({x, y, z});
    };
    printf("Vertices len: %lu\n", vertices.size());
    int a, b, c;
    while (fscanf(f, "f %d %d %d\n", &a, &b, &c) >= 3) {
        faces.push_back({a, b, c});
    };
    if (fscanf(f, "f %d %d %d", &a, &b, &c) >= 3) {
        faces.push_back({a, b, c});
    }

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
        // render.drawShape(vertices, indexArr, angle, {255, 0, 0, 255});
        render.drawModel(vertices, faces, angle);
        // render.drawModel(vertices, faces, M_PI);
        SDL::SDL_UpdateWindowSurface(render.inner_window);
    }
    loop_end:

    return 0;
}
