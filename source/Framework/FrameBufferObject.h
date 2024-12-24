#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

//#include "IwGL.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


// FrameBufferObject encapsulates an OpenGL ES framebuffer object
class FrameBufferObject
{
public:
  FrameBufferObject(int width, int height, GLenum format, GLenum type);
  ~FrameBufferObject();
  void Bind();
  void Release();
  GLuint GetTextureHandle() {return m_Tex;}

private:
  int m_Width;
  int m_Height;
  GLuint m_Tex;
  GLuint m_FBO;
  GLuint m_Depth;

  GLint mOrigViewport[4];
};


#endif