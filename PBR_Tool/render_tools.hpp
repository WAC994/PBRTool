#pragma once
#include <glm/glm.hpp>
#include "utilities.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "screen.hpp"

namespace pt
{
    void drawCube(bool flipfaces = false);
    void drawCubeSide(uint8_t face, bool flipfaces = false);
    // Use a shader to generate faces of a cube map
    uint32_t renderToCubeMap(int resolution, uint32_t bitFormat, bool useMips = false, uint32_t mipMapNum = 0);
    uint32_t renderIBLPreFilter(int resolution, uint32_t bitFormat, Shader& iblprefiltershader);
    uint32_t generateBRDFLUT(int resolution);

    class Bloom
    {
    public:
        Bloom();
        GLuint output(GLuint hdr);
    private:
        static constexpr uint32_t s_samples = 8U;
        Shader m_downSample, m_upSample;
        Screen m_screen;
        TextureData m_hdrData;
        glm::vec2 m_res[s_samples];
        GLuint m_frameBuf, m_downTex, m_upTex;
        bool m_resChanged = true;
    };
}