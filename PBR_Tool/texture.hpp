#pragma once
#include <glad/gl.h>

namespace pt
{
    class Texture
    {
    public:
        Texture();
        ~Texture();
        GLuint getID() const { return m_textureID; }
    private:
        GLuint m_textureID;
    };
}