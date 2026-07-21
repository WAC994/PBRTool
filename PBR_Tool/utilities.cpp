#include "utilities.hpp"
#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

static GLuint CURRENT_VERTEX_ARRAY = 0;
static GLuint CURRENT_FRAME_BUFFER = 0;
static GLuint CURRENT_VIEW_WIDTH = 0, CURRENT_VIEW_HEIGHT = 0;
static bool transformUBOCreated = false;
GLuint transformUBO;
namespace pt
{
    void bindVertexArray(GLuint vArray)
    {
        if (CURRENT_VERTEX_ARRAY != vArray)
        {
            glBindVertexArray(vArray);
            CURRENT_VERTEX_ARRAY = vArray;
        }
    }
    void bindFramebuffer(GLuint fbuffer)
    {
        if (CURRENT_FRAME_BUFFER != fbuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
            CURRENT_FRAME_BUFFER = fbuffer;
        }
    }

    void setViewPort(GLint width, GLint height)
    {
        if (CURRENT_VIEW_WIDTH != width && CURRENT_VIEW_HEIGHT != height)
        {
            glViewport(0, 0, width, height);
            CURRENT_VIEW_WIDTH = width;
            CURRENT_VIEW_HEIGHT = height;
        }
    }

    void setMeshTransform(const glm::mat4& transform)
    {
        if (!transformUBOCreated)
        {
            glCreateBuffers(1, &transformUBO);
            glNamedBufferStorage(transformUBO, sizeof(ModelMatrices), nullptr, GL_DYNAMIC_STORAGE_BIT);
            glBindBufferBase(GL_UNIFORM_BUFFER, TRANSFORM_UBO_BINDING_POINT, transformUBO);
            transformUBOCreated = true;
        }
        ModelMatrices data = { transform, glm::transpose(glm::inverse(transform)) };
        glNamedBufferSubData(transformUBO, 0, sizeof(ModelMatrices), &data);
    }
}