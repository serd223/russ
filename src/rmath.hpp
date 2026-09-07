#pragma once

namespace rmath {
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
        Vec3 cross(Vec3 rhs);
        float squarelen();
        float len();
        Vec3 normalize();
    };

    Vec3 operator + (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator - (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator / (const Vec3& lhs, const float& rhs);
    float operator * (const Vec3& lhs, const Vec3& rhs);
    Vec3 operator * (const Vec3& lhs, const float& rhs);
    Vec3 operator * (const float& lhs, const Vec3& rhs);

    class iVec3 {
        public:
        int a, b, c;
    };

    class Face {
        public:
        iVec3 indices;
        Vec3 normal;
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
}
