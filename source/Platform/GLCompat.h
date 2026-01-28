#pragma once

#ifdef __APPLE__
  // OpenGL desktop su macOS
  #include <OpenGL/gl.h>
  #include <OpenGL/glext.h>

  // --- OpenGL ES compat (OES -> desktop) ---
  #ifndef GL_FRAMEBUFFER_OES
    #define GL_FRAMEBUFFER_OES GL_FRAMEBUFFER
  #endif
  #ifndef GL_RENDERBUFFER_OES
    #define GL_RENDERBUFFER_OES GL_RENDERBUFFER
  #endif
  #ifndef GL_DEPTH_ATTACHMENT_OES
    #define GL_DEPTH_ATTACHMENT_OES GL_DEPTH_ATTACHMENT
  #endif

  // Funzioni OES -> core
  #ifndef glFramebufferRenderbufferOES
    #define glFramebufferRenderbufferOES glFramebufferRenderbuffer
  #endif

  // Orthof (float) -> Ortho (double)
  #ifndef glOrthof
    #define glOrthof(l,r,b,t,n,f) glOrtho((GLdouble)(l),(GLdouble)(r),(GLdouble)(b),(GLdouble)(t),(GLdouble)(n),(GLdouble)(f))
  #endif
#endif
