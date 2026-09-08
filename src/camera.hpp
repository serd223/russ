#pragma once

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

    Camera(Vec3 rot);

    private:
    Vec3 m_rot;
    Vec3 m_up;
    Vec3 m_right;
    Vec3 m_front;
};
