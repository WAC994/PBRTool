#pragma once
#include "utilities.hpp"
#include "shader.hpp"

namespace pt
{
    void drawCube(bool flipfaces = false);
    void drawCubeSide(uint8_t face, bool flipfaces = false);
    // Use a shader to generate faces of a cube map
    uint32_t renderToCubeMap(int resolution, uint32_t bitFormat, bool useMips = false, uint32_t mipMapNum = 0);
    uint32_t renderIBLPreFilter(int resolution, uint32_t bitFormat, Shader& iblprefiltershader);
    uint32_t generateBRDFLUT(int resolution);
}