#pragma once
#include "utilities.hpp"

namespace pt
{
    uint32_t cubeMapRender(uint32_t resolution);

    void drawCube(bool flipfaces = false);
    void drawCubeSides(uint8_t face, bool flipfaces = false);
}