#pragma once

/*
    Structs like Vectors and functions that are related to them.
*/
#include <math.h>

struct Vec2 {
    public:
    float x, y;
};

struct iVec2 {
    public:
    int x, y;
};

class Vec3 {
    public:
    float x, y, z;
    constexpr Vec3() = default;
    constexpr Vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}
    
    constexpr Vec3 cross(Vec3 rhs) const {
        return {
            this->y * rhs.z - this->z * rhs.y,
            this->z * rhs.x - this->x * rhs.z,
            this->x * rhs.y - this->y * rhs.x
        };
    }
    constexpr float squarelen() const {
        return this->x * this->x + this->y * this->y + this->z * this->z;
    }
    constexpr float len() const {
        return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
    }
    constexpr Vec3 normalize() const {
        float l = len();
        return {x/l, y/l, z/l};
    }
};

constexpr Vec3 operator + (const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };   
}

constexpr Vec3 operator - (const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
    };   
}

constexpr Vec3 operator / (const Vec3& lhs, const float& rhs) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
    };
}

constexpr float operator * (const Vec3& lhs, const Vec3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

constexpr Vec3 operator * (const Vec3& lhs, const float& rhs) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
    };
}

constexpr Vec3 operator * (const float& lhs, const Vec3& rhs) {
    return {
        rhs.x * lhs,
        rhs.y * lhs,
        rhs.z * lhs,
    };
}

class iVec3 {
    public:
    int a, b, c;
};

class Mat3x3 {
    public:
    union {
        struct {
            float _00, _01, _02;
            float _10, _11, _12;
            float _20, _21, _22;
        };
        float xs[9];
        struct {
            float r1[3];
            float r2[3];
            float r3[3];
        };
    };

    static Mat3x3 rotXYZ(Vec3 rot);
    static Mat3x3 rotX(float angle);
    static Mat3x3 rotY(float angle);
    static Mat3x3 rotZ(float angle);
    const static Mat3x3 id();
};

Mat3x3 operator * (const Mat3x3& lhs, const Mat3x3& rhs);
Vec3 operator * (const Mat3x3& lhs, const Vec3& rhs);
