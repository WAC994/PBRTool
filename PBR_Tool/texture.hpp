#pragma once
#include <cstdint>
#include <string_view>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "utilities.hpp"

namespace pt
{
    struct TextureData
    {
        uint32_t width = 512;
        uint32_t height = 512;
        uint32_t channels = 3;
        uint32_t levels = 1;
        GLenum bitformat = GL_RGB8;
        GLenum format = GL_RGB;
        GLenum dataType = GL_UNSIGNED_BYTE;
    };
    TextureData& getTextureData(uint32_t texture);
    void loadGLFWIcon(GLFWwindow* window, const std::string_view& path);
    uint32_t gen2DTexture(int width, int height, uint32_t bitformat = GL_RGB8, uint32_t wrap = GL_CLAMP_TO_EDGE, uint32_t filter = GL_NEAREST, bool usemips = false, uint32_t mipmapnum = 0);
    uint32_t genRenderTexture(int width, int height, uint32_t bitformat = GL_DEPTH24_STENCIL8);
    uint32_t gen2DTextureFromFile(const std::string_view& path, uint32_t bitformat = GL_RGB8, uint32_t wrap = GL_REPEAT, bool usemips = true, uint32_t mipmapnum = 0);
    uint32_t genCubeMapTexture(int width, int height, uint32_t bitformat, bool usemips = false, uint32_t mipmapnum = 0);
    uint32_t genDepthStencilRenderBuffer(int width, int height);
    uint64_t genBindless2DTexture(int width, int height, uint32_t bitformat = GL_RGB8, uint32_t wrap = GL_CLAMP_TO_EDGE, uint32_t filter = GL_NEAREST, bool usemips = false, uint32_t mipmapnum = 0);
    uint64_t genBindless2DTextureFromFile(const std::string_view& path, uint32_t bitformat = GL_RGB8, uint32_t wrap = GL_REPEAT, bool usemips = true, uint32_t mipmapnum = 0);
    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> genGBufferTextures(int width, int height);
}