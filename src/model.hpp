#pragma once

/*
    Each model object is responsible for one .obj file.
*/

#include <vector>
#include <rmath.hpp>
#include <span>

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
