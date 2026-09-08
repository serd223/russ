#include <camera.hpp>
#include <rmath.hpp>
#include <math.h>

Camera::Camera(Vec3 rot) {
    this->rot(rot);
}

Vec3 Camera::rot() const {
    return m_rot;
}

void Camera::rot(Vec3 newRot) {
    m_rot = newRot;
    float pitch = newRot.x, yaw = newRot.y;
    m_front.x = cosf(pitch) * sinf(yaw);
    m_front.y = sinf(pitch);
    m_front.z = cosf(pitch) * cosf(yaw) * -1;
    m_front = m_front.normalize();
    //
    m_right = m_front.cross(Vec3(0,1,0)).normalize();
    m_up = m_right.cross(m_front);
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
