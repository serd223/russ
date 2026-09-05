#include <rmath.hpp>

namespace rmath {

    Vec3 Vec3::cross(Vec3 rhs) {
        return {
            this->y * rhs.z - this->z * rhs.y,
            this->z * rhs.x - this->x * rhs.z,
            this->x * rhs.y - this->y * rhs.x
        };
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

    Vec3 operator * (const Vec3& lhs, const Vec3& rhs) {
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
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
}
