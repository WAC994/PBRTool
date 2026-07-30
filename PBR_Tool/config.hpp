#pragma once
#include <string>
#include <cstdint>

namespace pt
{
    struct Config
    {
        std::string GPURenderer = "Unknown";
        int32_t width = 1280;
        int32_t height = 720;
        bool fullscreen = false;
        bool vSync = false;
        bool hideCursor = true;
        bool debug = true;
    };
    void readConfig(Config& config);
    void writeConfig(Config& config);
}