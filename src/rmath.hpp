#pragma once

/*
    Structs like Vectors and functions that are related to them.
*/

#include <math.h>
#include <xsimd/xsimd.hpp>

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

class xVec3 {
    public:
    xsimd::batch<float> x, y, z;
    static inline xVec3 from_vec(const Vec3& v) {
        return {
            {v.x, v.x, v.x, v.x},
            {v.y, v.y, v.y, v.y},
            {v.z, v.z, v.z, v.z},
        };
    }

    inline xVec3 cross(xVec3 rhs) const {
        return {
            xsimd::fms(this->y, rhs.z, (this->z * rhs.y)),
            xsimd::fms(this->z, rhs.x, (this->x * rhs.z)),
            xsimd::fms(this->x, rhs.y, (this->y * rhs.x))
        };
    }

    inline Vec3 get(size_t i) const {
        return {
            x.get(i),
            y.get(i),
            z.get(i)
    };
}

};

inline xVec3 operator + (const xVec3& lhs, const xVec3& rhs) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };   
}

inline xVec3 operator - (const xVec3& lhs, const xVec3& rhs) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
    };   
}

inline xsimd::batch<float> operator * (const xVec3& lhs, const xVec3& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
inline xVec3 operator * (const xVec3& lhs, const float& rhs) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
    };
}
inline xVec3 operator * (const float& lhs, const xVec3& rhs) {
    return {
        rhs.x * lhs,
        rhs.y * lhs,
        rhs.z * lhs,
    };
}

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

    static constexpr Mat3x3 rotX(float angle) {
        return {
            .r1 = {1, 0          , 0           },
            .r2 = {0, cosf(angle), -sinf(angle)},
            .r3 = {0, sinf(angle), cosf(angle) },
        };
    }

    static constexpr Mat3x3 rotY(float angle) {
        return {
            .r1 = {cosf(angle) , 0, sinf(angle)},
            .r2 = {0           , 1, 0          },
            .r3 = {-sinf(angle), 0, cosf(angle)},
        };
    }

    static constexpr Mat3x3 rotZ(float angle) {
        return {
            .r1 = {cosf(angle), -sinf(angle), 0},
            .r2 = {sinf(angle), cosf(angle) , 0},
            .r3 = {0          , 0           , 1},
        };
    }

    static constexpr Mat3x3 id() {
        return {
            .xs = {
                1, 0, 0,
                0, 1, 0,
                0, 0, 1,
            }
        };
    }
};

constexpr Mat3x3 operator * (const Mat3x3& lhs, const Mat3x3& rhs) {
    return {
        ._00 = lhs._00 * rhs._00 + lhs._01 * rhs._10 + lhs._02 * rhs._20,
        ._01 = lhs._00 * rhs._01 + lhs._01 * rhs._11 + lhs._02 * rhs._21,
        ._02 = lhs._00 * rhs._02 + lhs._01 * rhs._12 + lhs._02 * rhs._22,
        ._10 = lhs._10 * rhs._00 + lhs._11 * rhs._10 + lhs._12 * rhs._20,
        ._11 = lhs._10 * rhs._01 + lhs._11 * rhs._11 + lhs._12 * rhs._21,
        ._12 = lhs._10 * rhs._02 + lhs._11 * rhs._12 + lhs._12 * rhs._22,
        ._20 = lhs._20 * rhs._00 + lhs._21 * rhs._10 + lhs._22 * rhs._20,
        ._21 = lhs._20 * rhs._01 + lhs._21 * rhs._11 + lhs._22 * rhs._21,
        ._22 = lhs._20 * rhs._02 + lhs._21 * rhs._12 + lhs._22 * rhs._22,
    };
}

static constexpr Mat3x3 Mat3x3RotXYZ(Vec3 rot) {
    return Mat3x3::rotX(rot.x) * Mat3x3::rotY(rot.y) * Mat3x3::rotZ(rot.z);
}

constexpr Vec3 operator * (const Mat3x3& lhs, const Vec3& rhs) {
    return {
        rhs.x * lhs.r1[0] + rhs.y * lhs.r1[1] + rhs.z * lhs.r1[2],
        rhs.x * lhs.r2[0] + rhs.y * lhs.r2[1] + rhs.z * lhs.r2[2],
        rhs.x * lhs.r3[0] + rhs.y * lhs.r3[1] + rhs.z * lhs.r3[2],
    };
}

inline xVec3 operator * (const Mat3x3& lhs, const xVec3& rhs) {
    return {
        rhs.x * lhs.r1[0] + rhs.y * lhs.r1[1] + rhs.z * lhs.r1[2],
        rhs.x * lhs.r2[0] + rhs.y * lhs.r2[1] + rhs.z * lhs.r2[2],
        rhs.x * lhs.r3[0] + rhs.y * lhs.r3[1] + rhs.z * lhs.r3[2],
    };
}

