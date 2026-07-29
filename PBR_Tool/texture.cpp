#include "texture.hpp"
#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_map>
#include <array>

namespace pt
{
    static std::map<GLuint, TextureData> s_imageCache;
    static std::map<const char*, GLuint> s_imageTextureCache;

    
    TextureData& getTextureData(uint32_t texture)
    {
        return s_imageCache[texture];
    }
    
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
    
    
    enum class IF // Image format
    {
        _8,
        _16,
        _f
    };
    
    struct GLTextureData
    {
        GLenum format;
        GLenum dataType;
        int channels;
        IF imageFormat;
    };
    
    std::unordered_map<GLenum, GLTextureData> s_GLTextureDataMap = {
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
    
    void storeTextureData(uint32_t texture, int width, int height, uint32_t levels, uint32_t bitformat)
    {
        TextureData cacheItem;
        auto glTextureData = s_GLTextureDataMap[bitformat];
        cacheItem.width = width;
        cacheItem.height = height;
        cacheItem.channels = glTextureData.channels;
        cacheItem.levels = levels;
        cacheItem.bitformat = bitformat;
        cacheItem.format = glTextureData.format;
        cacheItem.dataType = glTextureData.dataType;
        s_imageCache[texture] = cacheItem;
    }

    static void* getImage(const char* path, int* width, int* height, const GLTextureData& data)
    {
        void* imageData = nullptr;
        int Channels;
        switch (data.imageFormat)
        {
            case IF::_8:
            imageData = stbi_load(path, width, height, &Channels, data.channels);
            break;
            case IF::_16:
            imageData = stbi_load_16(path, width, height, &Channels, data.channels);
            break;
            case IF::_f:
            imageData = stbi_loadf(path, width, height, &Channels, data.channels);
            break;
        }
        if (imageData)
        return imageData;
        else
        {
            std::cerr << "Error: Failed to load image from " << path << '\n';
            return nullptr;
        }
    }
    
    uint32_t gen2DTextureFromFile(const std::string_view& path, uint32_t bitformat, uint32_t wrap, bool usemips, uint32_t mipmapnum)
    {
        const char* imagePath = path.data();
        
        if (s_imageTextureCache.find(imagePath) == s_imageTextureCache.end())
        {
            stbi_set_flip_vertically_on_load(true);
            GLuint texture;
            int width, height;
            auto glTextureData = s_GLTextureDataMap[bitformat];
            void* imageData = getImage(imagePath, &width, &height, glTextureData);
            glCreateTextures(GL_TEXTURE_2D, 1, &texture);

            GLsizei levels = 0;
            if (usemips && mipmapnum > 0)
                levels = mipmapnum;
            else if (usemips)
                levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))));
            else
                levels = 1;

            glTextureStorage2D(texture, levels, bitformat, width, height);
            glTextureSubImage2D(texture, 0, 0, 0, width, height, glTextureData.format, glTextureData.dataType, imageData);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap);
            glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (usemips)
                glGenerateTextureMipmap(texture);
            stbi_image_free(imageData);

            storeTextureData(texture, width, height, levels, bitformat);

            return texture;
        }
        return s_imageTextureCache[imagePath];
    }

    uint32_t gen2DTexture(int width, int height, uint32_t bitformat, uint32_t wrap, uint32_t filter, bool usemips, uint32_t mipmapnum)
    {
        auto glTextureData = s_GLTextureDataMap[bitformat];
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        GLsizei levels = 0;
        if (usemips && mipmapnum > 0)
            levels = mipmapnum;
        else if (usemips)
            levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))));
        else
            levels = 1;

        glTextureStorage2D(texture, levels, bitformat, width, height);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, usemips ? GL_LINEAR_MIPMAP_LINEAR : filter);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, filter);
        if (usemips)
            glGenerateTextureMipmap(texture);

        storeTextureData(texture, width, height, levels, bitformat);

        return texture;
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
    
    uint32_t genCubeMapTexture(int width, int height, uint32_t bitformat, bool usemips, uint32_t mipmapnum)
    {
        uint32_t cubeMap;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubeMap);
        
        GLsizei levels = 0;
        if (usemips && mipmapnum > 0)
        levels = mipmapnum;
        else if (usemips)
        levels = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(width, height))));
        else
        levels = 1;
        
        glTextureStorage2D(cubeMap, levels, bitformat, width, height);

        storeTextureData(cubeMap, width, height, levels, bitformat);

        return cubeMap;
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
        uint32_t texture = gen2DTexture(width, height, bitFormat, warp, filter, useMips, mipMapNum);
        uint64_t bindlessTexture = glGetTextureHandleARB(texture);
        glMakeTextureHandleResidentARB(bindlessTexture);
        return bindlessTexture;
    }

    uint64_t genBindless2DTextureFromFile(const std::string_view& path, uint32_t bitFormat, uint32_t warp, bool useMips, uint32_t mipMapNum)
    {
        uint32_t texture = gen2DTextureFromFile(path, bitFormat, warp, useMips, mipMapNum);
        uint64_t bindlessTexture = glGetTextureHandleARB(texture);
        glMakeTextureHandleResidentARB(bindlessTexture);
        return bindlessTexture;
    }

    std::array<uint32_t, MAX_G_BUFFER_TEXTURES> genGBufferTextures(int width, int height)
    {
        return
        {
            // Color
            genRenderTexture(width, height, GL_SRGB8_ALPHA8),
            // ORMS
            genRenderTexture(width, height, GL_RGBA8),
            // Emission
            genRenderTexture(width, height, GL_R11F_G11F_B10F),
            // Postion
            genRenderTexture(width, height, GL_RGB16F),
            // Normal
            genRenderTexture(width, height, GL_RGB16F),
            // View Direction
            genRenderTexture(width, height, GL_RGB16F)
        };
    }
}