#include "texture.hpp"

namespace pt
{
    Texture::Texture()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    }
    Texture::~Texture()
    {
        glDeleteTextures(1, &m_textureID);
    }
}