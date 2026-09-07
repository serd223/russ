#include <math.h>

#include "rmath.hpp"


Vec3 Vec3::cross(Vec3 rhs) {
    return {
        this->y * rhs.z - this->z * rhs.y,
        this->z * rhs.x - this->x * rhs.z,
        this->x * rhs.y - this->y * rhs.x
    };
}

float Vec3::squarelen() {
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

float Vec3::len() {
    return sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vec3 Vec3::normalize() {
    return *this * (1.0f / this->len());
}

Vec3 operator + (const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };   
}

Vec3 operator - (const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
    };   
}

Vec3 operator / (const Vec3& lhs, const float& rhs) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
    };
}

float operator * (const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs.x * rhs.x +
        lhs.y * rhs.y +
        lhs.z * rhs.z
    };   
}

Vec3 operator * (const Vec3& lhs, const float& rhs) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
    };
}

Vec3 operator * (const float& lhs, const Vec3& rhs) {
    return {
        rhs.x * lhs,
        rhs.y * lhs,
        rhs.z * lhs,
    };
}

Mat3x3 operator * (const Mat3x3& lhs, const Mat3x3& rhs) {
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

Vec3 operator * (const Mat3x3& lhs, const Vec3& rhs) {
    return {
        rhs.x * lhs.r1[0] + rhs.y * lhs.r1[1] + rhs.z * lhs.r1[2],
        rhs.x * lhs.r2[0] + rhs.y * lhs.r2[1] + rhs.z * lhs.r2[2],
        rhs.x * lhs.r3[0] + rhs.y * lhs.r3[1] + rhs.z * lhs.r3[2],
    };
}

Mat3x3 Mat3x3::rotX(float angle) {
    return {
        .r1 = {1, 0          , 0           },
        .r2 = {0, cosf(angle), -sinf(angle)},
        .r3 = {0, sinf(angle), cosf(angle) },
    };
}

Mat3x3 Mat3x3::rotY(float angle) {
    return {
        .r1 = {cosf(angle) , 0, sinf(angle)},
        .r2 = {0           , 1, 0          },
        .r3 = {-sinf(angle), 0, cosf(angle)},
    };
}

Mat3x3 Mat3x3::rotZ(float angle) {
    return {
        .r1 = {cosf(angle), -sinf(angle), 0},
        .r2 = {sinf(angle), cosf(angle) , 0},
        .r3 = {0          , 0           , 1},
    };
}

Mat3x3 Mat3x3::rotXYZ(Vec3 rot) {
    return Mat3x3::rotX(rot.x) * Mat3x3::rotY(rot.y) * Mat3x3::rotZ(rot.z);
}

const Mat3x3 Mat3x3::id() {
    return {
        .xs = {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        }
    };
}
