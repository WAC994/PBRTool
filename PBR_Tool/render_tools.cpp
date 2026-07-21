#include "render_tools.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pt
{
    static float cubeVertices[24] = {
        // Positions
        -0.5f, -0.5f,  0.5f, // 0
         0.5f, -0.5f,  0.5f, // 1
         0.5f,  0.5f,  0.5f, // 2
        -0.5f,  0.5f,  0.5f, // 3
         0.5f, -0.5f, -0.5f, // 4
        -0.5f, -0.5f, -0.5f, // 5
        -0.5f,  0.5f, -0.5f, // 6
         0.5f,  0.5f, -0.5f  // 7
    };

    static uint32_t cubeIndices[36] = {
        1, 4, 7, 1, 7, 2, // Right Face +X
        5, 0, 3, 5, 3, 6, // Left Face -X
        3, 2, 7, 3, 7, 6, // Top Face +Y
        1, 0, 5, 1, 5, 4, // Bottom Face -Y
        0, 1, 2, 0, 2, 3, // Front Face +Z
        4, 5, 6, 4, 6 ,7  // Back Face -Z
    };

    static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    static glm::mat4 captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    uint32_t cubeMapRender(uint32_t resolution)
    {
        return 0;
    }

    static bool cubeCreated = false;
    GLuint cubeVA, cubeVB, cubeIB;
    static void createCube()
    {
        glCreateVertexArrays(1, &cubeVA);
        GLuint buffers[2];
        glCreateBuffers(2, buffers);
        cubeVB = buffers[0];
        cubeIB = buffers[1];

        // Vertices
        glNamedBufferData(cubeVB, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexArrayVertexBuffer(cubeVA, 0, cubeVB, 0, 3 * sizeof(float));
        // Elements
        glNamedBufferData(cubeIB, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
        glVertexArrayElementBuffer(cubeVA, cubeIB);

        // Cube Vertices
        glEnableVertexArrayAttrib(cubeVA, 0);
        glVertexArrayAttribFormat(cubeVA, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(cubeVA, 0, 0);
        cubeCreated = true;
    }

    void drawCube(bool flipfaces)
    {
        if (!cubeCreated)
        {
            createCube();
        }

        setMeshTransform();
        
        bindVertexArray(cubeVA);
        if (flipfaces)
            glDisable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        if (flipfaces)
            glEnable(GL_CULL_FACE);
    }

    void drawCubeSides(uint8_t face, bool flipfaces)
    {
        if (!cubeCreated)
        {
            createCube();
        }

        setMeshTransform();

        if (face >= 6)
            face = 6;
        glBindVertexArray(cubeVA);
        if (flipfaces)
            glDisable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<const void*>(face * 6 * sizeof(uint32_t)));
        if (flipfaces)
            glEnable(GL_CULL_FACE);
    }
}