#include "inputs.hpp"
#include <GLFW/glfw3.h>
#include "pbr_tool_core.hpp"

namespace pt::Input
{
    auto window = App::getInstance().getWindow();

    glm::vec2 axis2D(AxisInput axisInput)
    {
        float X = 0.0f, Y = 0.0f;
        X -= static_cast<float>(glfwGetKey(window, (axisInput == AxisInput::WASD) ? GLFW_KEY_A : GLFW_KEY_LEFT));
        Y += static_cast<float>(glfwGetKey(window, (axisInput == AxisInput::WASD) ? GLFW_KEY_W : GLFW_KEY_UP));
        X += static_cast<float>(glfwGetKey(window, (axisInput == AxisInput::WASD) ? GLFW_KEY_D : GLFW_KEY_RIGHT));
        Y -= static_cast<float>(glfwGetKey(window, (axisInput == AxisInput::WASD) ? GLFW_KEY_S : GLFW_KEY_DOWN));
        return (X == 0.0f && Y == 0.0f) ? glm::vec2(X, Y) : glm::normalize(glm::vec2(X, Y));
    }

    glm::vec2 mousePosition()
    {
        double X, Y;
        glfwGetCursorPos(window, &X, &Y);
        return glm::vec2(static_cast<float>(X), static_cast<float>(Y));
    }

    bool keyPressed(int key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool keyReleased(int key)
    {
        return glfwGetKey(window, key) == GLFW_RELEASE;
    }
} // namespace Input
