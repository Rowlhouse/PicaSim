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

     $Id: puFLTK.h,v 1.1 2004/02/16 13:49:03 stromberg Exp $
*/

#ifndef _PU_FLTK_H_
#define _PU_FLTK_H_

#ifndef PU_USE_FLTK
# define PU_USE_FLTK
#endif

#include "pu.h"
#include <FL/Fl_Gl_Window.H>


inline int puGetWindowFLTK ()
{
    return (int) Fl_Window::current () ;
}

inline void puSetWindowFLTK ( int window )
{
    ((Fl_Gl_Window *) window) -> make_current () ;    
}

inline void puGetWindowSizeFLTK ( int *width, int *height )
{
    Fl_Window * window = Fl_Window::current () ;
    *width  = window->w() ;
    *height = window->h() ;
}

inline void puSetWindowSizeFLTK ( int width, int height )
{
    Fl_Window * window = Fl_Window::current () ;
    window -> resize ( window->x(), window->y(), width, height ) ;
}

inline void puInitFLTK ()
{
    puSetWindowFuncs ( puGetWindowFLTK,
		       puSetWindowFLTK,
		       puGetWindowSizeFLTK,
		       puSetWindowSizeFLTK ) ;
    puRealInit () ;
}


#endif
