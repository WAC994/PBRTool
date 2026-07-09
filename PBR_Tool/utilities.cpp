#include <utilities.hpp>

static GLuint CURRENT_VERTEX_ARRAY = 0;
static GLuint CURRENT_FRAME_BUFFER = 0;

namespace pt
{
    void bindVertexArray(GLuint vArray)
    {
        if (CURRENT_VERTEX_ARRAY != vArray)
        {
            glBindVertexArray(vArray);
            CURRENT_VERTEX_ARRAY = vArray;
        }
    }
    void bindFramebuffer(GLuint fbuffer)
    {
        if (CURRENT_FRAME_BUFFER != fbuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbuffer);
            CURRENT_FRAME_BUFFER = fbuffer;
        }
    }
}