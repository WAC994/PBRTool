#pragma once
#include <cstdint>
#include <glad/gl.h>

struct DrawElementsIndirectCommand
{
    DrawElementsIndirectCommand() = default;
    DrawElementsIndirectCommand(uint32_t c, uint32_t ic, uint32_t fi, int32_t bv, uint32_t bi)
        : count(c), instanceCount(ic), firstIndex(fi), basevertex(bv), baseInstance(bi)
    {
    }
    uint32_t count = 0;
    uint32_t instanceCount = 1;
    uint32_t firstIndex = 0;
    int32_t basevertex = 0;
    uint32_t baseInstance = 0;
};

using DEIC = DrawElementsIndirectCommand;

namespace pt
{
    void bindVertexArray(GLuint vArray);
    void bindFramebuffer(GLuint framebuffer);
    void setViewPort(GLint width, GLint height);

    // Shader buffer binding points
    static constexpr uint32_t CAMERA_UBO_BINDING_POINT = 0;
    static constexpr uint32_t TRANSFORM_SSBO_BINDING_POINT = 1;
    static constexpr uint32_t MATERIAL_SSBO_BINDING_POINT = 2;

    // Global array sizes
    static constexpr uint32_t MAX_FRAMEBUFFER_ATTACHMENTS = 32;
    static constexpr uint32_t MAX_MATERIALS = 1024;

    // Vertex array binding locations
    static constexpr GLuint POSITION_LOCATION = 4;
    static constexpr GLuint NORMAL_LOCATION = 3;
    static constexpr GLuint TANGENT_LOCATION = 2;
    static constexpr GLuint TEXTURE_COORDINATE_LOCATION = 1;
    static constexpr GLuint MATERIAL_LOCATION = 0;
}