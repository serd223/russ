#include "camera.hpp"
#include "rmath.hpp"

Vec3 Camera::rot() const {
    return m_rot;
}

void Camera::rot(Vec3 newRot) {
    m_rot = newRot;
    m_front = Mat3x3::rotXYZ(m_rot) * Vec3(0,0,-1);
    m_up = Mat3x3::rotXYZ(m_rot) * Vec3(0,1,0);
    m_right = Mat3x3::rotXYZ(m_rot) * Vec3(1,0,0);
}

Vec3 Camera::front() const {
    return m_front;
    
}

Vec3 Camera::up() const {
    return m_up;
}

Vec3 Camera::right() const {
    return m_right;
}