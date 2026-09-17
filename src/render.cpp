#include <errno.h>
#include <string.h>
#include <algorithm>
#include <cstddef>
#include <math.h>
#include <span>
#include <stdexcept>
#include <stdio.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <stdexcept>


namespace SDL {
    #include <SDL3/SDL.h>
}

#include <render.hpp>
#include <xsimd/xsimd.hpp>

typedef SDL::SDL_Color Color;

Point Renderer::_intersection(Point& currentP, Point& nextP, iVec2& clipEdge) {
    if (clipEdge.x == 1 && clipEdge.y == 0) {
        if (nextP.y - currentP.y == 0) {
            throw std::runtime_error("Division by 0.");
        }
        float t = -currentP.y / (float)(nextP.y - currentP.y);
        return Point {
            (int)round(currentP.x + (nextP.x - currentP.x) * t),
            0,
            currentP.z + (nextP.z - currentP.z) * t  
        };
    }
    if (clipEdge.x == 0 && clipEdge.y == 1) {
        if (nextP.x - currentP.x == 0) {
            throw std::runtime_error("Division by 0.");
        }
        float t = (inner_surface->w - currentP.x) / (float)(nextP.x - currentP.x);
        return Point {
            inner_surface->w,
            (int)round(currentP.y + (nextP.y - currentP.y) * t),
            currentP.z + (nextP.z - currentP.z) * t
        };
    }
    if (clipEdge.x == -1 && clipEdge.y == 0) {
        if (nextP.y - currentP.y == 0) {
            throw std::runtime_error("Division by 0.");
        }
        float t = (inner_surface->h - currentP.y) / (float)(nextP.y - currentP.y);
        return Point {
            (int)round(currentP.x + (nextP.x - currentP.x) * t),
            inner_surface->h,
            currentP.z + (nextP.z - currentP.z) * t  
        };
    }
    if (clipEdge.x == 0 && clipEdge.y == -1) {
        if (nextP.x - currentP.x == 0) {
            throw std::runtime_error("Division by 0.");
        }
        float t = -currentP.x / (float)(nextP.x - currentP.x);
        return Point {
            0,
            (int)round(currentP.y + (nextP.y - currentP.y) * t),
            currentP.z + (nextP.z - currentP.z) * t
        };
    }
    return {0, 0, 0};
}

bool Renderer::_inEdge(Point& p, iVec2& clipEdge) {
    if (clipEdge.x == 1 && clipEdge.y == 0) {
        return (p.y > 0);
    }
    if (clipEdge.x == 0 && clipEdge.y == 1) {
        return (p.x < inner_surface->w);
    }
    if (clipEdge.x == -1 && clipEdge.y == 0) {
        return (p.y < inner_surface->h);
    }
    if (clipEdge.x == 0 && clipEdge.y == -1) {
        return (p.x > 0);
    }
    return false;
}

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
        faces[i].normal = Mat3x3RotXYZ(m_rot) * l1.cross(l2).normalize();
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

// For xsimd vectorization
constexpr int chunk_len = 4;

void Renderer::drawModel(Model& model, Color tint, bool doLighting) {
    const float w = inner_surface->w, h = inner_surface->h;
    Vec3 position = model.pos - cam.pos;
    const Mat3x3 scaleMat = {
        .r1 = {model.scale, 0, 0},
        .r2 = {0, model.scale, 0},
        .r3 = {0, 0, model.scale},
    };
    Mat3x3 rot = Mat3x3RotXYZ(model.rot());
    Mat3x3 scaledRot = rot * scaleMat;

    // Don't move to camera space, just compute world coords relative to camera
    thread_local std::vector<Vec3> verticesWorld; // leak
    // Take each vertex and move it to camera space
    thread_local std::vector<Vec3> vertices; // leak
    verticesWorld.reserve(model.vertices.size());
    vertices.reserve(model.vertices.size());

    const xVec3 xcamRight = xVec3::from_vec(cam.right());
    const xVec3 xcamUp = xVec3::from_vec(cam.up());
    const xVec3 xcamFront = xVec3::from_vec(cam.front());
    const xVec3 xpos = xVec3::from_vec(position);
    static_assert(chunk_len == 4, "Chunk length changed!");
    const int chunk_count = model.vertices.size() / chunk_len;
    const int chunk_rem = model.vertices.size() % chunk_len;
    for (int chunk_i = 0; chunk_i < chunk_count; chunk_i++) {
        int i = chunk_i * chunk_len;
        const xVec3 v = {
            {model.vertices[i].x, model.vertices[i+1].x, model.vertices[i+2].x, model.vertices[i+3].x},
            {model.vertices[i].y, model.vertices[i+1].y, model.vertices[i+2].y, model.vertices[i+3].y},
            {model.vertices[i].z, model.vertices[i+1].z, model.vertices[i+2].z, model.vertices[i+3].z},
        };
        const xVec3 vWorld = (scaledRot * v) + xpos;
        const xVec3 vProj = {
            vWorld * xcamRight,
            vWorld * xcamUp,
            vWorld * xcamFront,
        };

        // The compiler would probably unroll anyway since chunk_len is constexpr but just to be sure
        verticesWorld[i + 0] = vWorld.get(0);
        vertices[i + 0] = vProj.get(0);
        verticesWorld[i + 1] = vWorld.get(1);
        vertices[i + 1] = vProj.get(1);
        verticesWorld[i + 2] = vWorld.get(2);
        vertices[i + 2] = vProj.get(2);
        verticesWorld[i + 3] = vWorld.get(3);
        vertices[i + 3] = vProj.get(3);
    }
    // for (size_t i = 0; i < model.vertices.size(); i++) {
    for (size_t i = model.vertices.size() - chunk_rem; i < model.vertices.size(); i++) {
        // Scale and offset vertices to world positions
        verticesWorld[i] = scaledRot * model.vertices[i] + position;
        vertices[i] = verticesWorld[i];
        vertices[i] = {vertices[i] * cam.right(), vertices[i] * cam.up(), vertices[i] * cam.front()};
        // vertices[i] = Mat3x3::rotXYZ({-cam.rot().x, -cam.rot().y, -cam.rot().z}) * vertices[i];
    }

    // Face culling
    thread_local std::vector<Face> faces; // leak
    faces.clear();
    for (auto& face : model.faces) {
        Vec3 v1 = verticesWorld[face.indices.a];
        Vec3 v2 = verticesWorld[face.indices.b];
        Vec3 v3 = verticesWorld[face.indices.c];

        // Back-face culling
        // Check if the angle between the ray from the camera to the middle of the ray and the face normal are pointing in different directions
        const float dot = face.normal * (v1 + v2 + v3);
        if (dot >= 0.0f) {
            continue;
        };

        v1 = vertices[face.indices.a];
        v2 = vertices[face.indices.b];
        v3 = vertices[face.indices.c];
        const Vec3 vecs[3] = {v1, v2, v3};
        // View Frustum culling
        if (!cam.draw(vecs)) continue;
        faces.push_back(face);
    }

    // Draw each face
    for (std::size_t i = 0; i < faces.size(); i++) {
        const Vec3 n = faces[i].normal;

        const Vec3 v1 = vertices[faces[i].indices.a];
        const Vec3 v2 = vertices[faces[i].indices.b];
        const Vec3 v3 = vertices[faces[i].indices.c];

        (void)tint;
        Color finalColor = model.color;
        if (doLighting) {
            // Fake lighting
            const float nMax = 0.65;
            if (n.y >= 0.0) {
                const float t = n.y > nMax ? nMax : n.y;
                finalColor.r = (int)((float)finalColor.r * (1.0 - t));
                finalColor.g = (int)((float)finalColor.g * (1.0 - t));
                finalColor.b = (int)((float)finalColor.b * (1.0 - t));
            }
            if (n.x >= 0.0) {
                const float t = n.x > nMax ? nMax : n.x;
                finalColor.r = (int)((float)finalColor.r * (1.0 - t));
                finalColor.g = (int)((float)finalColor.g * (1.0 - t));
                finalColor.b = (int)((float)finalColor.b * (1.0 - t));
            }
        }

        const float d = 1000.0;
        // On-screen positions of vertices // - on the y because actual y coordinates are flipped
        std::array<Point, 10> vOut = {
            Point {(int)(v1.x / v1.z * d) + inner_surface->w / 2, -(int)(v1.y / v1.z * d) + inner_surface->h / 2, v1.z},
            Point {(int)(v2.x / v2.z * d) + inner_surface->w / 2, -(int)(v2.y / v2.z * d) + inner_surface->h / 2, v2.z},
            Point {(int)(v3.x / v3.z * d) + inner_surface->w / 2, -(int)(v3.y / v3.z * d) + inner_surface->h / 2, v3.z},
        };
        int vOutSize = 3;

        //Sutherland-Hodgman Clipping Algorithm : https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
        const static std::array<iVec2, 4> clipEdges = {iVec2{1, 0}, iVec2{0, 1}, iVec2{-1, 0}, iVec2{0, -1}};
        for (auto clipEdge : clipEdges) {
            std::array<Point, 10> vIn = vOut;
            int vInSize = vOutSize;
            vOutSize = 0;

            for (int i = 0; i < vInSize; i++) {
                Point currentP = vIn[i];
                Point nextP = vIn[(i + 1) % (int)vInSize];
                Point interP;
                try {
                    interP = _intersection(currentP, nextP, clipEdge);
                } 
                catch (const std::runtime_error& e) {
                    if (_inEdge(nextP, clipEdge)) {
                        vOut[vOutSize] = nextP;
                        vOutSize++;
                    }
                    continue;
                }

                if (_inEdge(nextP, clipEdge)) {
                    if (!_inEdge(currentP, clipEdge)) {
                        vOut[vOutSize] = interP;
                        vOutSize++;
                    }
                    vOut[vOutSize] = nextP;
                    vOutSize++;
                }
                else if (_inEdge(currentP, clipEdge)) {
                    vOut[vOutSize] = interP;
                    vOutSize++;
                }
            }
        }

        // Draw triangles
        for (int i = 0; i < vOutSize - 2; i++) {
            Point vo1 = vOut[0];
            Point vo2 = vOut[i + 1];
            Point vo3 = vOut[i + 2];

            drawTriangleFilled( 
                Vec3(vo1.x, vo1.y, vo1.z),
                Vec3(vo2.x, vo2.y, vo2.z),
                Vec3(vo3.x, vo3.y, vo3.z),
                finalColor,
                w,
                h
            );
        }
    }
}

void Renderer::drawTriangleFilled(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2, Color color, float w, float h) {
    // Math explanation at:
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle//barycentric-coordinates.html

    Vec3 vertices[3] = {vertex0, vertex1, vertex2};
    if (vertices[1].y < vertices[0].y) std::swap(vertices[1], vertices[0]);
    if (vertices[2].y < vertices[0].y) std::swap(vertices[2], vertices[0]);
    if (vertices[2].y < vertices[1].y) std::swap(vertices[2], vertices[1]); // v2y > v1y > v0y
    const int miny = std::max(vertices[0].y, 0.0f);
    const int maxy = std::min(vertices[2].y, h-1);
    if (miny >= maxy) return;
    const int minx = std::max(std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x), 0.0f);
    const int maxx = std::min(std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x), w-1);
    if (minx >= maxx) return;
    static_assert(chunk_len == 4, "Chunk length changed!");
    const int row_len = maxx - minx + 1;
    const int chunk_count = row_len / chunk_len;
    const int chunk_rem = row_len % chunk_len;
    const xVec3 v0 = xVec3::from_vec(vertices[0]);
    const xVec3 v1 = xVec3::from_vec(vertices[1]);
    const xVec3 v2 = xVec3::from_vec(vertices[2]);
    const xVec3 v0v1 = v1 - v0;
    const xVec3 v0v2 = v2 - v0;
    const xVec3 N = v0v1.cross(v0v2);
    if (std::abs(N.z.first()) < 1e-9) return;
    // float inverse_nz = 1.0 / N.z;
    const xsimd::batch<float> inverse_N = xsimd::batch<float>{1.0f, 1.0f, 1.0f, 1.0f} / N.z;
    const xVec3 v1v2 = v2 - v1;
    const xVec3 v2v0 = v0 - v2;

    const SDL::Uint32 pixel = SDL::SDL_MapSurfaceRGB(inner_surface, color.r, color.g, color.b);
    static const xsimd::batch<float> xoffsets = {0.0, 1.0, 2.0, 3.0};
    const Vec3 n = N.get(0);
    const float in = inverse_N.get(0);
    const Vec3 _v0 = v0.get(0);
    const Vec3 _v1 = v1.get(0);
    const Vec3 _v2 = v2.get(0);
    const Vec3 _v0v1 = v0v1.get(0);
    const Vec3 _v1v2 = v1v2.get(0);
    const Vec3 _v2v0 = v2v0.get(0);

    // Each pixel can be processed in parallel without data races
    for (int y = miny; y <= maxy; y++) {
        SDL::Uint32* row = reinterpret_cast<SDL::Uint32*>(reinterpret_cast<unsigned char*>(inner_surface->pixels) + (y * inner_surface->pitch));
        float* z_row = z() + y * z_stride();
        for (int ix = 0; ix < chunk_count; ix++) {
            const int x0 = minx + chunk_len * ix;

            const float x0f = static_cast<float>(x0);
            const xsimd::batch<float> xf = xoffsets + x0f;
            const float yf = static_cast<float>(y);
            xVec3 p = {
                xf,
                {yf, yf, yf, yf},
                {0.0, 0.0, 0.0, 0.0}
            };
            // solve N * (p - v0) = 0
            p.z = xsimd::fnma((xsimd::fma(N.x, (p.x - v0.x), N.y * (p.y - v0.y))), inverse_N, v0.z);
            const xsimd::batch<float> existing_zs = xsimd::batch<float>::load_unaligned(&z_row[x0]);
            xsimd::batch_bool<float> mask = (p.z > 0.0f) && (p.z < existing_zs);
            if (xsimd::none(mask)) continue;

            const xVec3 v1p = p - v1;
            const xVec3 v2p = p - v2;
            const xVec3 v0p = p - v0;

            const xsimd::batch<float> dot0 = v1v2.cross(v1p) * N;
            const xsimd::batch<float> dot1 = v2v0.cross(v2p) * N;
            const xsimd::batch<float> dot2 = v0v1.cross(v0p) * N;

            mask = mask && (dot0 >= 0.0f) && (dot1 >= 0.0f) && (dot2 >= 0.0f);
            if (xsimd::none(mask)) continue;
            for (size_t i = 0; i < chunk_len; i++) {
                if (mask.get(i)) {
                    const int px = x0 + i;
                    z_row[px] = p.z.get(i);
                    row[px] = pixel;
                }
            }
        }
        if (chunk_rem == 0) continue;
        for (int x = maxx - chunk_rem + 1; x <= maxx; x++) {
            Vec3 p = {
                static_cast<float>(x),
                static_cast<float>(y),
                0
            };
            p.z = _v0.z - (n.x * (p.x - _v0.x) + n.y * (p.y - _v0.y)) * in;
            if (p.z <= 0.0f) continue;
            if (p.z >= z_row[x]) continue;

            const Vec3 v1p = p - _v1;
            const float dot0 = _v1v2.cross(v1p) * n;
            if (dot0 < 0) continue;

            const Vec3 v2p = p - _v2;
            const float dot1 = _v2v0.cross(v2p) * n;
            if (dot1 < 0) continue;

            const Vec3 v0p = p - _v0;
            const float dot2 = _v0v1.cross(v0p) * n;
            if (dot2 < 0) continue;

            z_row[x] = p.z;
            row[x] = pixel;
        }
    }
}
