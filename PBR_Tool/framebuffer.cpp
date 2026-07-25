#include "framebuffer.hpp"
#include <iostream>

namespace pt
{
    FrameBuffer::FrameBuffer()
    {
        glCreateFramebuffers(1, &m_frameBuffer);

        // Fill attachmentbuffer
        for (uint32_t i{}; i < 32; ++i)
        {
            m_colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }
    }

    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &m_frameBuffer);
    }

    void FrameBuffer::bind()
    {
        if (m_colorAttached & !m_drawBuffersSet)
        {
            glNamedFramebufferDrawBuffers(m_frameBuffer, m_colorAttachmentCount, m_colorAttachments);
            m_drawBuffersSet = true;
        }

        GLenum clearflag = m_colorAttached ? GL_COLOR_BUFFER_BIT : 0;
        clearflag |= m_depthAttached ? GL_DEPTH_BUFFER_BIT : 0;
        clearflag |= m_stencilAttached ? GL_STENCIL_BUFFER_BIT : 0;
        
        bindFramebuffer(m_frameBuffer);

        if (m_colorAttached || m_depthAttached || m_stencilAttached)
            glClear(clearflag);
    }

    void FrameBuffer::colorAttachment(uint32_t texture, uint32_t attachment, uint32_t miplevel)
    {
        m_colorAttached = true;
        if (attachment < MAX_FRAMEBUFFER_ATTACHMENTS)
        {
            glNamedFramebufferTexture(m_frameBuffer, GL_COLOR_ATTACHMENT0 + attachment, texture, miplevel);

            // If the attachment has been set before skip increasing the count
            if (m_colorAttachmentCache[attachment] == false)
            {
                m_colorAttachmentCount++;
                m_colorAttachmentCache[attachment] = true;
            }

            m_drawBuffersSet = false;
        }
    }

    void FrameBuffer::colorAttachments(uint32_t* textures, uint32_t attachment, uint32_t count, uint32_t miplevel)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            colorAttachment(textures[i], attachment + i, miplevel);
        }
    }

    void FrameBuffer::cubeMapAttachment(uint32_t cubemap, uint32_t attachment, uint32_t layer, uint32_t miplevel)
    {
        m_colorAttached = true;
        if (attachment < MAX_FRAMEBUFFER_ATTACHMENTS)
        {
            glNamedFramebufferTextureLayer(m_frameBuffer, attachment, cubemap, miplevel, layer);

            // If the attachment has been set before skip increasing the count
            if (m_colorAttachmentCache[attachment] == false)
            {
                m_colorAttachmentCount++;
                m_colorAttachmentCache[attachment] = true;
            }

            m_drawBuffersSet = false;
        }
    }

    void FrameBuffer::depthAttachment(uint32_t texture, uint32_t mipLevel)
    {
        m_depthAttached = true;
        glNamedFramebufferTexture(m_frameBuffer, GL_DEPTH_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::stencilAttachment(uint32_t texture, uint32_t mipLevel)
    {
        m_stencilAttached = true;
        glNamedFramebufferTexture(m_frameBuffer, GL_STENCIL_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::depthStencilAttachment(uint32_t texture, uint32_t mipLevel)
    {
        m_depthAttached = true;
        m_stencilAttached = true;
        glNamedFramebufferTexture(m_frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::blit(uint32_t drawbuffer, int32_t width, int32_t height, GLenum bitmask)
    {
        glBlitNamedFramebuffer(m_frameBuffer, drawbuffer, 0, 0, width, height, 0, 0, width, height, bitmask, GL_NEAREST);
    }

    void FrameBuffer::bindDefault()
    {
        bindFramebuffer(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}