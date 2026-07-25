#include "render_tools.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "framebuffer.hpp"
#include "texture.hpp"
#include "PBR_Tool_Core.hpp"

namespace pt
{
    static const float cubeVertices[24] = {
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

    static const uint32_t cubeIndices[36] = {
        1, 4, 7, 1, 7, 2, // Right Face +X
        5, 0, 3, 5, 3, 6, // Left Face -X
        3, 2, 7, 3, 7, 6, // Top Face +Y
        1, 0, 5, 1, 5, 4, // Bottom Face -Y
        0, 1, 2, 0, 2, 3, // Front Face +Z
        4, 5, 6, 4, 6, 7  // Back Face -Z
    };

    

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
        // Indices
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

    void drawCubeSide(uint8_t face, bool flipfaces)
    {
        if (!cubeCreated)
        {
            createCube();
        }

        setMeshTransform();

        if (face >= 6)
            face = 6;
        bindVertexArray(cubeVA);
        if (flipfaces)
            glDisable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<const void*>(face * 6 * sizeof(uint32_t)));
        if (flipfaces)
            glEnable(GL_CULL_FACE);
    }

    class CubeMapGenImpl
    {
    private:
    uint32_t m_cubeProjBuffer;
    bool m_isSetup = false;
    glm::mat4 m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    glm::mat4 m_captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    public:
        void setup()
        {
            if (!m_isSetup)
            {
                glCreateBuffers(1, &m_cubeProjBuffer);
                glNamedBufferStorage(m_cubeProjBuffer, sizeof(glm::mat4) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);
                glBindBufferBase(GL_UNIFORM_BUFFER, CUBE_SIDES_UBO_BINDING_POINT, m_cubeProjBuffer);
                m_isSetup = true;
            }
        }
        void updateProjection()
        {
            glNamedBufferSubData(m_cubeProjBuffer, 0, sizeof(glm::mat4), &m_captureProjection);
        }
        void updateView(uint32_t side)
        {
            glNamedBufferSubData(m_cubeProjBuffer, sizeof(glm::mat4), sizeof(glm::mat4), &m_captureViews[side]);
        }
        FrameBuffer framebuffer;
    };
    static CubeMapGenImpl cubeMapGenerator;

    uint32_t renderToCubeMap(int resolution, uint32_t bitFormat, bool useMips, uint32_t mipMapNum)
    {
        uint32_t cubeMap = genCubeMapTexture(resolution, resolution, bitFormat, useMips, mipMapNum);
        cubeMapGenerator.setup();

        // Set the viewport to the resolution
        glViewport(0, 0, resolution, resolution);
        
        // Update projection for capture
        cubeMapGenerator.updateProjection();
        
        for (uint32_t i = 0; i < 6; i++)
        {
            // Upate view of each cube side
            cubeMapGenerator.updateView(i);
            
            // Attach and bind the framebuffer cube map side to capture to
            cubeMapGenerator.framebuffer.cubeMapAttachment(cubeMap, 0, i);
            cubeMapGenerator.framebuffer.bind();
            
            // Draw Cube side
            drawCubeSide(i, true);
        }
        
        // Reset the viewport after capturing
        glViewport(0, 0, App::getInstance().config.width, App::getInstance().config.height);

        // Bind the defualt buffer so any onther glClear call doesnt affect the cube map
        FrameBuffer::bindDefault();

        // Return the filled cube map
        return cubeMap;
    }
}