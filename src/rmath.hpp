#pragma once

namespace rmath {
    typedef struct Vec3 {
        float x, y, z;
        struct Vec3 cross(struct Vec3 rhs); 
    } Vec3;

    Vec3 operator + (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator - (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator * (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator * (const Vec3& lhs, const float& rhs);
    Vec3 operator * (const float& lhs, const Vec3& rhs);

    typedef struct {
        int a, b, c;
    } Face;
}
