#pragma once
#include <string>

namespace pt
{
    struct Config
    {
        std::string GPURenderer = "Unknown";
        int width = 1280;
        int height = 720;
        bool enableVSync = false;
    };
}