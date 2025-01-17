
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

     $Id: puPW.h,v 1.1 2004/02/17 01:43:21 sjbaker Exp $
*/

#ifndef _PU_PW_H_
#define _PU_PW_H_

#include "pu.h"

#define PUPW_WINDOW_MAGIC  0x3DEB4938  /* Random! */

inline int puGetWindowPW()
{
  return PUPW_WINDOW_MAGIC ;
}

inline void puSetWindowPW ( int window )
{
  // Not possible because PW is a single-window library.
  // But we can at least check that the handle matches.

  assert ( window == PUPW_WINDOW_MAGIC ) ;
}

inline void puGetWindowSizePW ( int *width, int *height )
{
  pwGetSize ( width, height ) ;
}

inline void puSetWindowSizePW ( int width, int height )
{
  pwSetSize ( width, height ) ;
}

inline void puInitPW ()
{
  puSetWindowFuncs ( puGetWindowPW,
                     puSetWindowPW,
                     puGetWindowSizePW,
                     puSetWindowSizePW ) ;
  puRealInit () ;
}


#endif

