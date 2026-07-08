#include "screen.hpp"
#include <cstdint>
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

const float s_screenVertices[12] = {
    -1.0f, -1.0f, 0.0f, 0.0f, // 0
    3.0f, -1.0f, 2.0f, 0.0f,  // 1
    -1.0f, 3.0f, 0.0f, 2.0f   // 2
};
const char* s_screenVertShader =
"#version 450 core\n"
"/* IN */\n"
"layout(location = 0) in vec2 a_position;\n"
"layout(location = 1) in vec2 a_texCoord;\n"
"/* OUT */\n"
"layout(location = 0) out vec2 v_texCoord;\n"
"void main(void)\n"
"{\n"
"    gl_Position = vec4(a_position, 0.0, 1.0);\n"
"    v_texCoord = a_texCoord;\n"
"}\0";
const char* s_screenFragShader =
"#version 450 core\n"
"/* IN */\n"
"layout(location = 0) in vec2 v_texCoord;\n"
"/* OUT */\n"
"layout(location = 0) out vec4 f_color;\n"
"/* UNIFORMS AND BUFFERS */\n"
"layout(binding = 0) uniform sampler2D u_texture;\n"
"/* FUNCTIONS */\n"
"vec4 gammaCorrection(vec4 color)\n"
"{\n"
"   vec3 lowValue = color.rgb * 12.92;\n"
"   precise vec3 highValue = fma(vec3(1.055), pow(color.rgb, vec3(0.41666)), vec3(-0.055));\n"
"   color.rgb = mix(lowValue, highValue, greaterThan(color.rgb, vec3(0.0031308)));\n"
"   return color;\n"
"}\n"
"void main(void)\n"
"{\n"
"   f_color = gammaCorrection(texture(u_texture, v_texCoord));\n"
"}\0";

static void bindVertexArrayImpl(uint32_t vArray)
{
    static uint32_t currentArray = 0;
    if (currentArray != vArray)
    {
        glBindVertexArray(vArray);
        currentArray = vArray;
    }
}
namespace pt
{
    Screen::Screen()
    {
        // Create screen vertex array and buffer
        glCreateVertexArrays(1, &m_screenArray);
        glCreateBuffers(1, &m_screenBuffer);
        glVertexArrayVertexBuffer(m_screenArray, 0, m_screenBuffer, 0, 4 * sizeof(float));
        glNamedBufferData(m_screenBuffer, sizeof(s_screenVertices), s_screenVertices, GL_STATIC_DRAW);

        // Screen Vertices
        glEnableVertexArrayAttrib(m_screenArray, 0);
        glVertexArrayAttribFormat(m_screenArray, 0, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_screenArray, 0, 0);

        // Screen TexCoords
        glEnableVertexArrayAttrib(m_screenArray, 1);
        glVertexArrayAttribFormat(m_screenArray, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
        glVertexArrayAttribBinding(m_screenArray, 1, 0);

        // Create screen shader
        m_screenShader.addShader(ShaderType::VERT, s_screenVertShader, SHADERFILEINFO::STRING);
        m_screenShader.addShader(ShaderType::FRAG, s_screenFragShader, SHADERFILEINFO::STRING);
        m_screenShader.linkProgram();
    }

    Screen::~Screen()
    {
        glDeleteVertexArrays(1, &m_screenArray);
        glDeleteBuffers(1, &m_screenBuffer);
    }

    void Screen::draw()
    {
        bindVertexArrayImpl(m_screenArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Screen::draw(uint32_t texture)
    {
        m_screenShader.bindTexture(0, texture);
        draw();
    }
}