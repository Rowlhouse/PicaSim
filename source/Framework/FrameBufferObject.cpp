#include "FrameBufferObject.h"
#include "Graphics.h"
#include "../Platform/S3ECompat.h"

//======================================================================================================================
FrameBufferObject::FrameBufferObject(int width, int height, GLenum format, GLenum type)
{
    m_Width = width;
    m_Height = height;

    // Generate a texture for the frame buffer
    glGenTextures(1, &m_Tex);
    glBindTexture(GL_TEXTURE_2D, m_Tex);

    // Disable mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Reserve storage
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (gGLVersion == 1)
    {
        // Use the framebuffer extension to create a framebuffer object.
        // Note that functions can be called directly.
        glGenFramebuffersOES(1, &m_FBO);

        // Generate a depth buffer as a render buffer
        glGenRenderbuffersOES(1, &m_Depth);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_Depth);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, width, height);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, 0);

        // Bind both to the frame buffer
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_FBO);
        glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, m_Tex, 0);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, m_Depth);

        // Check the frame buffer for completeness
        IwAssert(Rowlhouse, glCheckFramebufferStatus(GL_FRAMEBUFFER_OES) == GL_FRAMEBUFFER_COMPLETE_OES);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
    }
    else
    {
        // Use the framebuffer extension to create a framebuffer object.
        // Note that functions can be called directly.
        glGenFramebuffers(1, &m_FBO);

        // Generate a depth buffer as a render buffer
        glGenRenderbuffers(1, &m_Depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Bind both to the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Depth);

        // Check the frame buffer for completeness
        IwAssert(Rowlhouse, glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

//======================================================================================================================
FrameBufferObject::~FrameBufferObject()
{
    // Clear up GL objects
    glDeleteTextures(1, &m_Tex);

    if (gGLVersion == 1)
    {
        glDeleteFramebuffersOES(1, &m_FBO);
        glDeleteRenderbuffersOES(1, &m_Depth);
    }
    else
    {
        glDeleteFramebuffers(1, &m_FBO);
        glDeleteRenderbuffers(1, &m_Depth);
    }
}

//======================================================================================================================
void FrameBufferObject::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // Must set viewport to framebuffer dimensions
    glGetIntegerv( GL_VIEWPORT, &mOrigViewport[0] );
    glViewport(0, 0, m_Width, m_Height);
}

//======================================================================================================================
void FrameBufferObject::Release()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(mOrigViewport[0], mOrigViewport[1], mOrigViewport[2], mOrigViewport[3]);
}
