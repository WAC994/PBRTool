#pragma once
#include <glm/glm.hpp>

namespace pt
{
    enum class AxisInput
    {
        WASD,
        ARROWS,
    };

    namespace Input
    {
        glm::vec2 axis2D(AxisInput axisInput = AxisInput::WASD);
        glm::vec2 mousePosition();
        bool keyPressed(int key);
        bool keyReleased(int key);
    }
}