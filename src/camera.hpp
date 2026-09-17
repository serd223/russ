#pragma once
#include <array>
#include <span>

/*
    Camera object, this is the POV of our Renderer. Camera's position in world coordinates,
    its rotation, etc. are handled here.
*/

#include <rmath.hpp>

class Camera {
    public:
    const Vec3 origin = {0, 0, 0};
    Vec3 pos = {0, 0, 0};
    constexpr Vec3 rot() const {
        return m_rot;
    };
    /// recalculates up, right, front
    void rot(Vec3 newRot);

    constexpr Vec3 front() const {
        return m_front;
    }

    constexpr Vec3 up() const {
        return m_up;
    }

    constexpr Vec3 right() const {
        return m_right;
    }

    constexpr float near() const {
        return m_near;
    }

    constexpr float far() const {
        return m_far;
    }

    bool draw(std::span<const Vec3, 3> vertices);
    
    Camera(Vec3 rot);
    
    private:
    std::array<Vec3,4> m_sn;
    Vec3 m_rot;
    Vec3 m_up;
    Vec3 m_right;
    Vec3 m_front;
    const float m_far = 500.0f;
    const float m_near = 6.2f;
    const float m_fc = 5.0;

};
