#pragma once
#include <utilities.hpp>

namespace pt
{
  class FrameBuffer
  {
  public:
    FrameBuffer();
    ~FrameBuffer();
    void bind();
    void colorAttachment(uint32_t texture, uint32_t attachment, uint32_t miplevel);
    void colorAttachments(uint32_t* textures, uint32_t attachment, uint32_t count, uint32_t miplevel);
    void depthAttachment(uint32_t texture, uint32_t mipLevel);
    void stencilAttachment(uint32_t texture, uint32_t mipLevel);
    void depthStencilAttachment(uint32_t texture, uint32_t mipLevel);
    static void bindDefault();

  private:
    uint32_t m_frameBuffer;
    uint32_t m_colorAttachmentCount{};
    uint32_t m_colorAttachments[MAX_FRAMEBUFFER_ATTACHMENTS];
    bool m_colorAttached = false;
    bool m_depthAttached = false;
    bool m_stencilAttached = false;
    bool m_drawBuffersSet = false;
  };
}