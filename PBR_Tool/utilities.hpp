#pragma once
#include <cstdint>
#include <glad/gl.h>

namespace pt
{
    void bindVertexArray(GLuint vArray);
    void bindFramebuffer(GLuint framebuffer);
    static constexpr uint32_t MAX_FRAMEBUFFER_ATTACHMENTS = 32;
    static constexpr uint32_t CAMERA_UBO_BINDING_POINT = 0;
    static constexpr uint32_t TRANSFORM_SSBO_BINDING_POINT = 1;
}