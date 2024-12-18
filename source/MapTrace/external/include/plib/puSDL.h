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

     $Id: puSDL.h,v 1.1 2004/02/16 13:49:03 stromberg Exp $
*/

#ifndef _PU_SDL_H_
#define _PU_SDL_H_

#ifndef PU_USE_SDL
# define PU_USE_SDL
#endif

#include "pu.h"
#include <SDL/SDL.h>


inline int puGetWindowSDL ()
{
    return 0;
}

inline void puGetWindowSizeSDL ( int *width, int *height )
{
    SDL_Surface *display = SDL_GetVideoSurface () ;
    *width  = display->w ;
    *height = display->h ;
}

inline void puInitSDL ()
{
    puSetWindowFuncs ( puGetWindowSDL,     NULL,
                       puGetWindowSizeSDL, NULL ) ;
    puRealInit () ;
}


#endif
