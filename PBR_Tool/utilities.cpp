#include <utilities.hpp>

static GLuint CURRENT_VERTEX_ARRAY = 0;
static GLuint CURRENT_FRAME_BUFFER = 0;
static GLuint CURRENT_VIEW_WIDTH = 0, CURRENT_VIEW_HEIGHT = 0;

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

    void setViewPort(GLint width, GLint height)
    {
        if (CURRENT_VIEW_WIDTH != width && CURRENT_VIEW_HEIGHT != height)
        {
            glViewport(0, 0, width, height);
            CURRENT_VIEW_WIDTH = width;
            CURRENT_VIEW_HEIGHT = height;
        }
    }
}