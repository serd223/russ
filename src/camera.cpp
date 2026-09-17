#include <camera.hpp>
#include <rmath.hpp>
#include <math.h>

Camera::Camera(Vec3 rot) {
    this->rot(rot);

    m_sn[0] = {0,  m_fc / sqrt(9 + m_fc * m_fc), -3 / sqrt(3 + m_fc * m_fc)};
    m_sn[1] = {-m_fc / sqrt(16 + m_fc * m_fc), 0, -4 / sqrt(16 + m_fc *m_fc )};
    m_sn[2] = {0, -m_fc / sqrt(9 + m_fc * m_fc), -3 / sqrt(9 + m_fc * m_fc)};
    m_sn[3] = {m_fc / sqrt(16 + m_fc * m_fc), 0, -4 / sqrt(16 + m_fc *m_fc )};
    
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

bool Camera::draw(std::span<const Vec3, 3> vertices) {
    // compiler should unroll
    for (auto& v : vertices) {
        bool plane = v.z > m_near && v.z < m_far; // Near and far plane
        bool side = false;
        for (size_t i = 0; i < m_sn.size(); i++) {
            // TODO : Not really working
            if (m_sn[i] * v < 0) side = true;
        }
        if (plane && side) return true;
    }
    return false;
}
