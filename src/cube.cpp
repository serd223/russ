#include "rmath.hpp"
using namespace rmath;

extern const Vec3 cube_vertices[8] = {
    { 0.5f,   0.5f,   0.5f},
    { 0.5f,   0.5f,  -0.5f},
    {-0.5f,   0.5f,  -0.5f},
    {-0.5f,   0.5f,   0.5f},
    {-0.5f,  -0.5f,   0.5f},
    { 0.5f,  -0.5f,   0.5f},
    { 0.5f,  -0.5f,  -0.5f},
    {-0.5f,  -0.5f,  -0.5f},
};

extern const iVec3 cube_faces[12] = {
    {0, 1, 2},
    {0, 2, 3},
    {5, 7, 6},
    {5, 4, 7},
    {0, 3, 4},
    {0, 4, 5},
    {1, 6, 7},
    {1, 7, 2},
    {0, 5, 6},
    {0, 6, 1},
    {3, 2, 7},
    {3, 7, 4},
};
