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

    typedef struct Mat3x3 {
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

        static struct Mat3x3 rotXYZ(Vec3 rot);
        static struct Mat3x3 rotX(float angle);
        static struct Mat3x3 rotY(float angle);
        static struct Mat3x3 rotZ(float angle);
    } Mat3x3;

    Mat3x3 operator * (const Mat3x3& lhs, const Mat3x3& rhs);
    Vec3 operator * (const Mat3x3& lhs, const Vec3& rhs);
}
