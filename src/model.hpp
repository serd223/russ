#pragma once

/*
    Each model object is responsible for one .obj file.
*/

#include <vector>
#include "rmath.hpp"
#include "rmath.hpp"

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