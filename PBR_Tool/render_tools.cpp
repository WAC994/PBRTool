#include "render_tools.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "PBR_Tool_Core.hpp"
#include "framebuffer.hpp"

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

    class RenderTools
    {
    private:
        uint32_t m_cubeProjBuffer;
        bool m_isSetup = false;
        glm::mat4 m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

        glm::mat4 m_captureViews[6] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
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
        Shader lutShader = Shader("assets/shaders/screen.vert", "assets/shaders/brdf_integration.frag", SHADERFILEINFO::TEXTFILE);
        FrameBuffer framebuffer;
        Screen screen;
    };

    static RenderTools tools;

    uint32_t renderToCubeMap(int resolution, uint32_t bitFormat, bool useMips, uint32_t mipMapNum)
    {
        uint32_t cubeMap = genCubeMapTexture(resolution, resolution, bitFormat, useMips, mipMapNum);
        tools.setup();

        // Set the viewport to the resolution
        glViewport(0, 0, resolution, resolution);

        // Update projection for capture
        tools.updateProjection();

        for (uint32_t i = 0; i < 6; i++)
        {
            // Upate view of each cube side
            tools.updateView(i);

            // Attach and bind the framebuffer cube map side to capture to
            tools.framebuffer.cubeMapAttachment(cubeMap, 0, i);
            tools.framebuffer.bind();

            // Draw cube side
            drawCubeSide(i, true);
        }

        // Generate mip maps after drawing
        if (useMips)
        {
            glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(cubeMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(cubeMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glGenerateTextureMipmap(cubeMap);
        }

        // Reset the viewport and bind defaut framebuffer after capturing
        glViewport(0, 0, App::getInstance().config.width, App::getInstance().config.height);
        FrameBuffer::bindDefault();

        // Return the filled cube map
        return cubeMap;
    }

    uint32_t renderIBLPreFilter(int resolution, uint32_t bitFormat, Shader& iblprefiltershader)
    {
        uint32_t cubeMap = genCubeMapTexture(resolution, resolution, bitFormat, true);

        // IBL prefilter parameters
        glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cubeMap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(cubeMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(cubeMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        tools.setup();
        uint32_t maxLevels = getTextureData(cubeMap).levels;

        // Update projection for capture
        tools.updateProjection();
        iblprefiltershader.setFloat(0, resolution);

        for (uint32_t mipLevel = 0; mipLevel < maxLevels; mipLevel++)
        {
            uint32_t mipResolution = resolution * std::pow(0.5f, mipLevel);

            // Set the viewport to the resolution of every mip map level
            glViewport(0, 0, mipResolution, mipResolution);

            // Calculate the roughness to send to the shader
            float roughness = (float)mipLevel / (float)(maxLevels - 1);
            float alpha2 = std::pow(roughness, 4.0f);

            // Send alpha2 to ibl prefilter shader
            iblprefiltershader.setFloat(1, alpha2);

            // Draw all side of the pre-filter cube map for each roughness level
            for (uint32_t i = 0; i < 6; i++)
            {
                // Upate view of each cube side
                tools.updateView(i);

                // Attach and bind the framebuffer cube map side to capture to
                tools.framebuffer.cubeMapAttachment(cubeMap, 0, i, mipLevel);
                tools.framebuffer.bind();

                // Draw cube side
                drawCubeSide(i, true);
            }
        }

        // Reset the viewport and bind defaut framebuffer after capturing
        glViewport(0, 0, App::getInstance().config.width, App::getInstance().config.height);
        FrameBuffer::bindDefault();

        return cubeMap;
    }

    uint32_t generateBRDFLUT(int resolution)
    {
        tools.setup();
        uint32_t lut = gen2DTexture(resolution, resolution, GL_RG16F);
        tools.framebuffer.colorAttachment(lut, 0);

        // Set viewport to resoulution
        glViewport(0, 0, resolution, resolution);

        // Draw LUT
        tools.framebuffer.bind();
        tools.lutShader.use();
        tools.screen.draw();

        // Reset the viewport and bind defaut framebuffer after capturing
        glViewport(0, 0, App::getInstance().config.width, App::getInstance().config.height);
        FrameBuffer::bindDefault();

        return lut;
    }

    Bloom::Bloom()
    {
        m_downSample.addShader(ShaderType::VERT, "assets/shaders/screen.vert", SHADERFILEINFO::TEXTFILE);
        m_downSample.addShader(ShaderType::FRAG, "assets/shaders/bloom_down_sample.frag", SHADERFILEINFO::TEXTFILE);
        m_downSample.linkProgram();
        m_upSample.addShader(ShaderType::VERT, "assets/shaders/screen.vert", SHADERFILEINFO::TEXTFILE);
        m_upSample.addShader(ShaderType::FRAG, "assets/shaders/bloom_up_sample.frag", SHADERFILEINFO::TEXTFILE);
        m_upSample.linkProgram();
        glCreateFramebuffers(1, &m_frameBuf);
    }

    GLuint Bloom::output(GLuint hdr)
    {
        auto& hdrDataNew = getTextureData(hdr);
        uint32_t lods = s_samples - 1;
        glm::vec2 bloomRes;

        if ((m_hdrData.width != hdrDataNew.width) || (m_hdrData.height != hdrDataNew.height))
        {
            m_hdrData = hdrDataNew;
            bloomRes = glm::vec2(m_hdrData.width, m_hdrData.height) / 2.0f;

            for (int32_t i = lods; i >= 0; i--)
            {
                m_res[i] = bloomRes / static_cast<float>(1 << i);
            }
            m_resChanged = true;
        }

        if (m_resChanged)
        {
            m_downTex = gen2DTexture(static_cast<int32_t>(bloomRes.x), static_cast<int32_t>(bloomRes.y), GL_R11F_G11F_B10F, GL_CLAMP_TO_EDGE, GL_LINEAR, true, s_samples);
            m_upTex = gen2DTexture(static_cast<int32_t>(bloomRes.x), static_cast<int32_t>(bloomRes.y), GL_R11F_G11F_B10F, GL_CLAMP_TO_EDGE, GL_LINEAR, true, s_samples);
            m_resChanged = false;
        }

        bindFramebuffer(m_frameBuf);
        
        // Downsample from the hdr texture to the down sample texture mips with a Kawase filter
        for (uint32_t i = 0U; i < s_samples; i++)
        {
            glViewport(0, 0, static_cast<int32_t>(m_res[i].x), static_cast<int32_t>(m_res[i].y));
            glNamedFramebufferTexture(m_frameBuf, GL_COLOR_ATTACHMENT0, m_downTex, i);
            m_downSample.bindTexture(0, (i == 0) ? hdr : m_downTex);
            m_downSample.setNamedFloat("u_lod", (i == 0) ? 0.0f : static_cast<float>(i - 1));
            m_downSample.setNamedVec2("u_texel", 1.0f / (m_res[i] * 2.0f));
            m_downSample.setNamedBool("u_karis_avg", i == 1);
            m_screen.draw();
        }

        // blur and sum to upsample texture
        for (int32_t i = lods; i >= 0; i--)
        {
            glViewport(0, 0, static_cast<int32_t>(m_res[i].x), static_cast<int32_t>(m_res[i].y));
            glNamedFramebufferTexture(m_frameBuf, GL_COLOR_ATTACHMENT0, m_upTex, i);
            m_upSample.bindTexture(0, m_downTex);
            m_upSample.bindTexture(1, m_upTex);
            m_upSample.setNamedFloat("u_blur_lod", static_cast<float>(i));
            m_upSample.setNamedFloat("u_add_lod", static_cast<float>(i + 1));
            m_upSample.setNamedFloat("u_block_add", (i == lods) ? 0.0f : 1.0f);
            m_screen.draw();
        }

        // Reset the viewport and bind defaut framebuffer after capturing
        glViewport(0, 0, App::getInstance().config.width, App::getInstance().config.height);
        FrameBuffer::bindDefault();

        return m_upTex;
    }
}