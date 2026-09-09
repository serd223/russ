#include <camera.hpp>
#include <cmath>
#include <cstring>
#include <rmath.hpp>
#include <math.h>

Camera::Camera(Vec3 rot) {
    this->rot(rot);
    const static float d = 5.0f;
    m_sn[0] = {0,  d / sqrt(9 + d * d), -3 / sqrt(3 + d * d)};
    m_sn[1] = {-d / sqrt(16 + d * d), 0, -4 / sqrt(16 + d *d )};
    m_sn[2] = {0, -d / sqrt(9 + d * d), -3 / sqrt(9 + d * d)};
    m_sn[3] = {d / sqrt(16 + d * d), 0, -4 / sqrt(16 + d *d )};
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

bool Camera::draw(std::vector<Vec3>& vertices) {

    for (size_t i = 0; i < m_sn.size(); i++) {
        Vec3 sn = m_sn[i];
        sn = Mat3x3::rotXYZ(m_rot) * sn;
        for (auto& v : vertices) {
            if (m_sn[0] * v > 0) return false;
            if (m_sn[1] * v > 0) return false;
            if (m_sn[2] * v > 0) return false;
            if (m_sn[3] * v > 0) return false;
        }
    }
    return true;
}
