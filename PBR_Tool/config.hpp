#pragma once
#include <string>

namespace pt
{
    struct Config
    {
        int width = 800;
        int height = 600;
        std::string GPURenderer = "Unknown";
    };
}