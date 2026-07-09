#include "framebuffer.hpp"

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

    void FrameBuffer::bind() const
    {
        glNamedFramebufferDrawBuffers(m_frameBuffer, m_colorAttachmentCount, m_colorAttachments);
        bindFramebuffer(m_frameBuffer);
    }

    void FrameBuffer::colorAttachment(uint32_t texture, uint32_t attachment, uint32_t miplevel)
    {
        if (attachment < MAX_FRAMEBUFFER_ATTACHMENTS)
        {
            glNamedFramebufferTexture(m_frameBuffer, GL_COLOR_ATTACHMENT0 + attachment, texture, miplevel);
            m_colorAttachmentCount++;
        }
    }

    void FrameBuffer::colorAttachments(uint32_t* textures, uint32_t attachment, uint32_t count, uint32_t miplevel)
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            if (attachment < MAX_FRAMEBUFFER_ATTACHMENTS)
            {
                glNamedFramebufferTexture(m_frameBuffer, attachment + i + GL_COLOR_ATTACHMENT0, textures[i], miplevel);
                m_colorAttachmentCount++;
            }
        }
    }

    void FrameBuffer::depthAttachment(uint32_t texture, uint32_t mipLevel) const
    {
        glNamedFramebufferTexture(m_frameBuffer, GL_DEPTH_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::stencilAttachment(uint32_t texture, uint32_t mipLevel) const
    {
        glNamedFramebufferTexture(m_frameBuffer, GL_STENCIL_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::depthStencilAttachment(uint32_t texture, uint32_t mipLevel) const
    {
        glNamedFramebufferTexture(m_frameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, texture, mipLevel);
    }

    void FrameBuffer::bindDefault()
    {
        bindFramebuffer(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}