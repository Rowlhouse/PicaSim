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

     $Id: puGLUT.h,v 1.1 2004/02/16 13:49:03 stromberg Exp $
*/

#ifndef _PU_GLUT_H_
#define _PU_GLUT_H_

#ifndef PU_USE_GLUT
# define PU_USE_GLUT
#endif

#include "pu.h"

#ifdef UL_MAC_OSX
# include <GLUT/glut.h>
#else
# include <GL/glut.h>
#endif


inline int puGetWindowGLUT()
{
    return glutGetWindow () ;
}

inline void puSetWindowGLUT ( int window )
{
    glutSetWindow ( window ) ; 
}

inline void puGetWindowSizeGLUT ( int *width, int *height )
{
    *width  = glutGet ( (GLenum) GLUT_WINDOW_WIDTH  ) ;
    *height = glutGet ( (GLenum) GLUT_WINDOW_HEIGHT ) ;
}

inline void puSetWindowSizeGLUT ( int width, int height )
{
    glutReshapeWindow ( width, height ) ;
}

inline void puInitGLUT ()
{
    puSetWindowFuncs ( puGetWindowGLUT,
		       puSetWindowGLUT,
		       puGetWindowSizeGLUT,
		       puSetWindowSizeGLUT ) ;
    puRealInit () ;
}


#endif
