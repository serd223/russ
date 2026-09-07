#include <cstdio>
#include <cerrno>
#include <cstring>
#include <stdio.h>

#include <SDL3/SDL.h>

#include "model.hpp"

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