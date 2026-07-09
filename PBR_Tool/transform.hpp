#pragma once
#include <glm/glm.hpp>

namespace pt
{
    struct ModelMatricesData
    {
        ModelMatricesData(const glm::mat4& transformmatrix, const glm::mat4& normalmatrix)
            : transformMatrix(transformmatrix), normalMatrix(normalmatrix)
        {
        }
        glm::mat4 transformMatrix = glm::mat4(1.0f);
        glm::mat4 normalMatrix = glm::mat4(1.0f);
    };

    struct Transform
    {
        void translate(const glm::vec3& value);
        void rotate(float angle, const glm::vec3& value);
        void scale(const glm::vec3& value);
        ModelMatricesData getMartices();
    protected:
        glm::mat4 transform = glm::mat4(1.0f);
    };
}