#pragma once
#include <utilities.hpp>
#include <map>

namespace pt
{
  class FrameBuffer
  {
  public:
    FrameBuffer();
    ~FrameBuffer();
    void bind();
    inline uint32_t getHandle() const { return m_frameBuffer; }
    void colorAttachment(uint32_t texture, uint32_t attachment, uint32_t miplevel = 0);
    void colorAttachments(uint32_t* textures, uint32_t attachment, uint32_t count, uint32_t miplevel = 0);
    void cubeMapAttachment(uint32_t cubemap, uint32_t attachment, uint32_t layer, uint32_t miplevel = 0);
    void depthAttachment(uint32_t texture, uint32_t mipLevel = 0);
    void stencilAttachment(uint32_t texture, uint32_t mipLevel = 0);
    void depthStencilAttachment(uint32_t texture, uint32_t mipLevel = 0);
    void blit(uint32_t drawbuffer, int32_t width, int32_t height, GLenum bitmask);
    static void bindDefault();
  private:
    std::map<uint32_t, bool> m_colorAttachmentCache;
    uint32_t m_frameBuffer;
    uint32_t m_colorAttachmentCount = 0;
    uint32_t m_colorAttachments[MAX_FRAMEBUFFER_ATTACHMENTS];
    bool m_colorAttached = false;
    bool m_depthAttached = false;
    bool m_stencilAttached = false;
    bool m_drawBuffersSet = false;
  };
}