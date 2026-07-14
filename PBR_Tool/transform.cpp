#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace pt
{
    void Transform::translate(const glm::vec3& value)
    {
        transform = glm::translate(transform, value);
    }

    void Transform::rotate(float angle, const glm::vec3& value)
    {
        transform = glm::rotate(transform, glm::radians(angle), value);
    }

    void Transform::scale(const glm::vec3& value)
    {
        transform = glm::scale(transform, value);
    }

    ModelMatrices Transform::getMartices()
    {
        return { transform, glm::transpose(glm::inverse(transform)) };
    }
}