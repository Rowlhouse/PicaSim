/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

     For further information visit http://plib.sourceforge.net

     $Id: puNative.h,v 1.1 2004/02/16 13:49:03 stromberg Exp $
*/

#ifndef _PU_NATIVE_H_
#define _PU_NATIVE_H_

#ifndef PU_USE_NATIVE
# define PU_USE_NATIVE
#endif

#include "pu.h"

#if defined(UL_GLX)
# include <GL/glx.h>
#elif defined(UL_WGL)
// nothing
#elif defined(UL_AGL)
# include <agl.h>
#elif defined(UL_CGL)
# include <OpenGL/CGLCurrent.h>
#endif


inline int puGetWindowNative ()
{
#if defined(UL_GLX)
  return (int) glXGetCurrentDrawable () ;
#elif defined(UL_WGL)
  return (int) wglGetCurrentDC () ;
#elif defined(UL_AGL)
  return (int) aglGetCurrentDrawable () ;
#elif defined(UL_CGL)
  return (int) CGLGetCurrentContext () ;
#else
  return 0 ;
#endif
}


inline void puGetWindowSizeNative ( int *width, int *height )
{
#if defined(UL_GLX)

    Window root ;
    int x, y ;
    unsigned int w, h, b, d ;

    XGetGeometry ( glXGetCurrentDisplay  (),
		   glXGetCurrentDrawable (),
		   &root, &x, &y, &w, &h, &b, &d ) ;
    *width  = w ;
    *height = h ;

#elif defined(UL_WGL)

    RECT r ;
    GetClientRect( WindowFromDC( wglGetCurrentDC() ), &r );
    *width  = r.right ;
    *height = r.bottom ;

#else // Help! Need implementations for more systems.

    GLint vp[4] ;
    glGetIntegerv ( GL_VIEWPORT, vp ) ;
    *width  = vp[0] + vp[2] ;
    *height = vp[1] + vp[3] ;
    // Note: puSetOpenGLState calls glViewport(0, 0, w, h).

#endif
}


inline void puInitNative ()
{
    puSetWindowFuncs ( puGetWindowNative,     NULL,
		       puGetWindowSizeNative, NULL ) ;
    puRealInit () ;
}


#endif
