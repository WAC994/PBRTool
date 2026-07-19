#pragma once
#include <cstdint>
#include <string_view>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "utilities.hpp"

namespace pt
{
    void loadGLFWIcon(GLFWwindow* window, const std::string_view& path);
    uint32_t gen2DTexture(int width, int height, uint32_t bitFormat = GL_RGB8, uint32_t wrap = GL_CLAMP_TO_EDGE, uint32_t filter = GL_NEAREST, bool useMips = false, uint32_t mipMapNum = 0);
    uint32_t genRenderTexture(int width, int height, uint32_t bitFormat = GL_DEPTH24_STENCIL8);
    uint32_t gen2DTextureFromFile(const std::string_view& path, uint32_t bitFormat = GL_RGB8, uint32_t wrap = GL_REPEAT, bool useMips = true, uint32_t mipMapNum = 0);
    uint32_t genCubeMapTexture(int width, int height, uint32_t bitFormat, bool useMips = false, uint32_t mipMapNum = 0);
    uint32_t genDepthStencilRenderBuffer(int width, int height);
    uint64_t genBindless2DTexture(int width, int height, uint32_t bitFormat = GL_RGB8, uint32_t wrap = GL_CLAMP_TO_EDGE, uint32_t filter = GL_NEAREST, bool useMips = false, uint32_t mipMapNum = 0);
    uint64_t genBindless2DTextureFromFile(const std::string_view& path, uint32_t bitFormat = GL_RGB8, uint32_t wrap = GL_REPEAT, bool useMips = true, uint32_t mipMapNum = 0);
    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> genGBufferTextures(int width, int height);
}