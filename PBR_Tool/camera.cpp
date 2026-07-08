#include "camera.hpp"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

constexpr float CAMERA_YAW = -90.0f;
constexpr float CAMERA_PITCH = 0.0f;
constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr float ASPECT_16_BY_9 = 1.777f;
constexpr uint32_t CAMERA_UBO_BINDING_POINT = 0;

namespace pt
{
    Camera::Camera(const CameraSettings& config)
        : CameraSettings(config), m_aspectRatio{ ASPECT_16_BY_9 }, m_yaw{ CAMERA_YAW }, m_pitch{ CAMERA_PITCH },
        m_maxFieldOfView{ fieldOfView }
    {
        updateVectors();

        glCreateBuffers(1, &m_cameraBuffer);
        glNamedBufferStorage(m_cameraBuffer, sizeof(CameraData), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UBO_BINDING_POINT, m_cameraBuffer);
    }

    Camera::~Camera()
    {
        glDeleteBuffers(1, &m_cameraBuffer);
    }

    void Camera::move(const glm::vec2& input, float deltaTime)
    {
        glm::vec2 velocity = glm::vec2(input.x, input.y) * (speed * deltaTime);
        position += m_front * velocity.y;
        position += m_right * velocity.x;
        update();
    }

    void Camera::rotate(float inputX, float inputY)
    {
        m_yaw += inputX * sensitivity;

        m_pitch += inputY * sensitivity;

        // Limit vertical camera rotation (pitch)
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        // Update the camera vectors using the new yaw and pitch
        updateVectors();
        update();
    }

    void Camera::zoom(float input)
    {
        fieldOfView += input;

        // Limit zoom (feild of view) to the range of 1.0 and the set field of view
        if (fieldOfView < 1.0f)
            fieldOfView = 1.0f;
        if (fieldOfView > m_maxFieldOfView)
            fieldOfView = m_maxFieldOfView;
        update();
    }

    CameraData Camera::getData()
    {
        return { getPerspectiveMatrix(), getViewMatrix(), position };
    }

    void Camera::updateVectors()
    {
        m_front =
            glm::normalize(glm::vec3(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)), sin(glm::radians(m_pitch)),
                sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))));

        m_right = glm::normalize(glm::cross(m_front, WORLD_UP));

        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

    void Camera::update()
    {
        auto cameraData = getData();
        glNamedBufferSubData(m_cameraBuffer, 0, sizeof(CameraData), &cameraData);
    }

    glm::mat4 Camera::getPerspectiveMatrix() const
    {
        return glm::perspective(glm::radians(fieldOfView), m_aspectRatio, nearPlane, farPlane);
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(position, position + m_front, m_up);
    }

    void Camera::setAspectRatio(float aspectRatio)
    {
        m_aspectRatio = aspectRatio;
    }
}