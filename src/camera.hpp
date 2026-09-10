#pragma once
#include <vector>
#include <array>

/*
    Camera object, this is the POV of our Renderer. Camera's position in world coordinates,
    its rotation, etc. are handled here.
*/

#include <rmath.hpp>

class Camera {
    public:
    const Vec3 origin = {0, 0, 0};
    Vec3 pos = {0, 0, 0};
    Vec3 rot() const;
    /// recalculates up, right, front
    void rot(Vec3 newRot);
    Vec3 front() const;
    Vec3 up() const;
    Vec3 right() const;
    bool draw(std::vector<Vec3>& vertices);
    
    Camera(Vec3 rot);
    
    private:
    std::array<Vec3,4> m_sn;
    Vec3 m_rot;
    Vec3 m_up;
    Vec3 m_right;
    Vec3 m_front;
    const float m_far = 500.0f;
    const float m_near = 6.2f;
    const float m_fc = 6.0f;

};
