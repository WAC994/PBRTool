#pragma once
#include "shader.hpp"
#include <cstdint>

namespace pt
{
    class Screen
    {
    public:
        Screen();
        ~Screen();

    public:
        void draw();
        void draw(uint32_t texture);

    private:
        uint32_t m_screenArray{};
        uint32_t m_screenBuffer{};
        Shader m_screenShader;
    };
}