#pragma once
#include <glm/glm.hpp>
#include "utilities.hpp"

namespace pt
{
    struct CameraData
    {
        glm::mat4 perspective, view;
        glm::vec3 position;
    };

    class Camera
    {
    public:
        Camera();
        ~Camera();
        void move(const glm::vec2& input, float deltaTime);
        void rotate(float inputX, float inputY);
        void zoom(float input);
        glm::mat4 getPerspectiveMatrix() const;
        glm::mat4 getViewMatrix() const;
        CameraData getData();
        void setAspectRatio(float aspectRatio);
        void update();
    private:
        void updateVectors();
    public:
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
        float fieldOfView = 55.0f;
        float nearPlane = 0.01f;
        float farPlane = 100.0f;
        float speed = 1.0f;
        float sensitivity = 0.05f;
    private:
        glm::vec3 m_front, m_right, m_up;
        float m_aspectRatio, m_yaw, m_pitch, m_maxFieldOfView;
        uint32_t m_buffer;
    };
}