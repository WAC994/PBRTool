#pragma once
#include "shader.hpp"
#include "utilities.hpp"

namespace pt
{
    class Screen
    {
    public:
        Screen();
        ~Screen();
    public:
        void draw();
        void draw(GLuint texture);
    private:
        GLuint m_vArray, m_vBuffer;
        Shader m_shader;
    };
}