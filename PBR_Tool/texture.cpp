#include "texture.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>

void loadGLFWIcon(GLFWwindow* window, const std::string_view& path)
{
    GLFWimage image;
    auto data = stbi_load(path.data(), &image.width, &image.height, nullptr, 4);
    if (!data)
    {
        std::cerr << "Error: Failed to icon " << path.data() << '.' << std::endl;
        return;
    }
    image.pixels = data;
    glfwSetWindowIcon(window, 1, &image);
    stbi_image_free(data);
}

static std::map<const char*, GLuint> s_imageTextureCache;

enum class IF // Image format
{
    _8,
    _16,
    _f
};

struct GLTextureData
{
    GLenum Format;
    GLenum DataType;
    int Channels;
    IF IFormat;
};

std::unordered_map<GLenum, GLTextureData> GLTexData = {
    // 8 bits per channel
    {GL_R8, GLTextureData{GL_RED, GL_UNSIGNED_BYTE, 1, IF::_8}},
    {GL_RG8, GLTextureData{GL_RG, GL_UNSIGNED_BYTE, 2, IF::_8}},
    {GL_RGB8, GLTextureData{GL_RGB, GL_UNSIGNED_BYTE, 3, IF::_8}},
    {GL_RGBA8, GLTextureData{GL_RGBA, GL_UNSIGNED_BYTE, 4, IF::_8}},
    // 16 bits per channel
    {GL_R16, GLTextureData{GL_RED, GL_UNSIGNED_SHORT, 1, IF::_16}},
    {GL_RG16, GLTextureData{GL_RG, GL_UNSIGNED_SHORT, 2, IF::_16}},
    {GL_RGB16, GLTextureData{GL_RGB, GL_UNSIGNED_SHORT, 3, IF::_16}},
    {GL_RGBA16, GLTextureData{GL_RGBA, GL_UNSIGNED_SHORT, 4, IF::_16}},
    // 16 bits per channel float
    {GL_R16F, GLTextureData{GL_RED, GL_FLOAT, 1, IF::_f}},
    {GL_RG16F, GLTextureData{GL_RG, GL_FLOAT, 2, IF::_f}},
    {GL_RGB16F, GLTextureData{GL_RGB, GL_FLOAT, 3, IF::_f}},
    {GL_RGBA16F, GLTextureData{GL_RGBA, GL_FLOAT, 4, IF::_f}},
    // 32 bits per channel float
    {GL_R32F, GLTextureData{GL_RED, GL_FLOAT, 1, IF::_f}},
    {GL_RG32F, GLTextureData{GL_RG, GL_FLOAT, 2, IF::_f}},
    {GL_RGB32F, GLTextureData{GL_RGB, GL_FLOAT, 3, IF::_f}},
    {GL_RGBA32F, GLTextureData{GL_RGBA, GL_FLOAT, 4, IF::_f}},
    // sRGB
    {GL_SRGB8, GLTextureData{GL_RGB, GL_UNSIGNED_BYTE, 3, IF::_8}},
    {GL_SRGB8_ALPHA8, GLTextureData{GL_RGBA, GL_UNSIGNED_BYTE, 4, IF::_8}},
    // 9 bits RGB, 5 bits exponent
    {GL_RGB9_E5, GLTextureData{GL_RGB, GL_FLOAT, 3, IF::_f}},
    // 11 bits Red 11 bits green 10 bits blue
    {GL_R11F_G11F_B10F, GLTextureData{GL_RGB, GL_FLOAT, 3, IF::_f}}
};

static void* getImage(const char* path, int* width, int* height, const GLTextureData& data)
{
    void* Data = nullptr;
    int Channels;
    switch (data.IFormat)
    {
    case IF::_8:
        Data = stbi_load(path, width, height, &Channels, data.Channels);
        break;
    case IF::_16:
        Data = stbi_load_16(path, width, height, &Channels, data.Channels);
        break;
    case IF::_f:
        Data = stbi_loadf(path, width, height, &Channels, data.Channels);
        break;
    }
    if (Data)
        return Data;
    else
    {
        std::cerr << "Error: Failed to load image from " << path << '\n';
        return nullptr;
    }
}

uint32_t gen2DTextureFromFile(const std::string_view& path, uint32_t bitFormat, uint32_t wrap, bool useMips,
    uint32_t mipMapNum)
{
    const char* Path = path.data();

    if (s_imageTextureCache.find(Path) == s_imageTextureCache.end())
    {
        stbi_set_flip_vertically_on_load(true);
        GLuint Texture;
        int Width, Height;
        auto TextureData = GLTexData[bitFormat];
        void* Image = getImage(Path, &Width, &Height, TextureData);
        glCreateTextures(GL_TEXTURE_2D, 1, &Texture);

        GLsizei Levels = 0;
        if (useMips && mipMapNum > 0)
            Levels = mipMapNum;
        else if (useMips)
            Levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(Width, Height))));
        else
            Levels = 1;

        glTextureStorage2D(Texture, Levels, bitFormat, Width, Height);
        glTextureSubImage2D(Texture, 0, 0, 0, Width, Height, TextureData.Format, TextureData.DataType, Image);
        glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, wrap);
        glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (useMips)
            glGenerateTextureMipmap(Texture);
        stbi_image_free(Image);
        return Texture;
    }
    return s_imageTextureCache[Path];
}

uint32_t gen2DTexture(int width, int height, uint32_t bitFormat, uint32_t wrap, uint32_t filter, bool useMips,
    uint32_t mipMapNum)
{
    GLuint Texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &Texture);

    GLsizei Levels = 0;
    if (useMips && mipMapNum > 0)
        Levels = mipMapNum;
    else if (useMips)
        Levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))));
    else
        Levels = 1;

    glTextureStorage2D(Texture, Levels, bitFormat, width, height);
    glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, wrap);
    glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, useMips ? GL_LINEAR_MIPMAP_LINEAR : filter);
    glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, filter);
    if (useMips)
        glGenerateTextureMipmap(Texture);
    return Texture;
}

uint32_t genRenderTexture(int width, int height, uint32_t bitFormat)
{
    GLuint RenderTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &RenderTexture);
    glTextureStorage2D(RenderTexture, 1, bitFormat, width, height);
    glTextureParameteri(RenderTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(RenderTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return RenderTexture;
}

uint32_t genCubeMapTexture(int width, int height, uint32_t bitFormat, bool useMips, uint32_t mipMapNum)
{
    uint32_t CubeMap;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &CubeMap);

    GLsizei Levels = 0;
    if (useMips && mipMapNum > 0)
        Levels = mipMapNum;
    else if (useMips)
        Levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))));
    else
        Levels = 1;

    glTextureStorage2D(CubeMap, Levels, bitFormat, width, height);
    if (useMips)
        glGenerateTextureMipmap(CubeMap);
    return CubeMap;
}

uint32_t genDepthStencilRenderBuffer(int width, int height)
{
    GLuint RenderBuffer;
    glCreateRenderbuffers(1, &RenderBuffer);
    glNamedRenderbufferStorage(RenderBuffer, GL_DEPTH24_STENCIL8, width, height);
    return RenderBuffer;
}

uint64_t genBindless2DTexture(int width, int height, uint32_t bitFormat, uint32_t warp, uint32_t filter, bool useMips, uint32_t mipMapNum)
{
    return glGetTextureHandleARB(gen2DTexture(width, height, bitFormat, warp, filter, useMips, mipMapNum));
}

uint64_t genBindless2DTextureFromFile(const std::string_view& path, uint32_t bitFormat, uint32_t warp, bool useMips, uint32_t mipMapNum)
{
    return glGetTextureHandleARB(gen2DTextureFromFile(path, bitFormat, warp, useMips, mipMapNum));
}

std::map<GLuint, TextureData> s_textureCache;

Texture::Texture(int width, int height, uint32_t bitFormat, uint32_t warp, uint32_t filter, bool useMips, uint32_t mipMapNum)
{
    handle = gen2DTexture(width, height, bitFormat, warp, filter, useMips, mipMapNum);
    handle64 = glGetTextureHandleARB(handle);
    glMakeTextureHandleResidentARB(handle64);
    s_textureCache[handle] = { width, height, GLTexData[bitFormat].Channels };
}

Texture::Texture(const std::string_view& path, uint32_t bitFormat, uint32_t warp, bool useMips, uint32_t mipMapNum)
{
    handle = gen2DTextureFromFile(path, bitFormat, warp, useMips, mipMapNum);
    handle64 = glGetTextureHandleARB(handle);
    glMakeTextureHandleResidentARB(handle64);
    s_textureCache[handle] = { s_textureCache[handle].width, s_textureCache[handle].height, GLTexData[bitFormat].Channels };
}

Texture::~Texture()
{
    if (handle)
        glDeleteTextures(1, &handle);
    if (handle64)
        glMakeTextureHandleNonResidentARB(handle64);
    s_textureCache.erase(handle);
}