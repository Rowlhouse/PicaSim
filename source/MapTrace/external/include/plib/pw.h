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

     $Id: pw.h,v 1.7 2004/04/06 23:45:19 sjbaker Exp $
*/


#ifndef _PW_H_
#define _PW_H_ 1

#include <stdio.h>

#define PW_IS_PRESENT  1

typedef void pwResizeCB ( int w, int h ) ;
typedef void pwExitCB   () ;
typedef void pwKeybdFunc    ( int key, int updown, int x, int y ) ;
typedef void pwMouseFunc    ( int key, int updown, int x, int y ) ;
typedef void pwMousePosFunc ( int x, int y ) ;

void pwInit ( int multisample, int num_samples ) ;

void pwInit ( int x, int y, int w, int h,
              int multisample,
              char *title, int border, int num_samples ) ;

void pwSetCallbacks ( pwKeybdFunc    *kb = NULL, pwMouseFunc *ms = NULL,
                      pwMousePosFunc *mp = NULL, pwResizeCB *rcb = NULL,
                      pwExitCB      *ecb = NULL ) ;

int  pwGetModifiers  () ;
void pwSwapBuffers   () ;
void pwGetSize       ( int *w, int *h ) ;
void pwSetSizeOrigin ( int x, int y, int w, int h ) ;
void pwSetSize       ( int x, int y ) ;
void pwSetOrigin     ( int w, int h ) ;
void pwSetCursor     ( int c ) ;
void pwCleanup       () ;
void pwSetAutoRepeatKey ( bool enable ) ;

#define PW_CURSOR_NONE    0
#define PW_CURSOR_RIGHT   1
#define PW_CURSOR_LEFT    2
#define PW_CURSOR_QUERY   3
#define PW_CURSOR_AIM     4
#define PW_CURSOR_CIRCLE  5
#define PW_CURSOR_WAIT    6
#define PW_CURSOR_CROSS   7
#define PW_CURSOR_CUSTOM  8

/*
  WARNING: These have to be the same as PU_KEY_whatever.
*/

#define PW_KEY_SPECIAL_OFFSET  256

#define PW_KEY_F1        (1  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F2        (2  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F3        (3  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F4        (4  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F5        (5  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F6        (6  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F7        (7  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F8        (8  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F9        (9  + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F10       (10 + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F11       (11 + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_F12       (12 + PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_LEFT      (100+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_UP        (101+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_RIGHT     (102+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_DOWN      (103+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_PAGE_UP   (104+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_PAGE_DOWN (105+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_HOME      (106+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_END       (107+ PW_KEY_SPECIAL_OFFSET)
#define PW_KEY_INSERT    (108+ PW_KEY_SPECIAL_OFFSET)

/* The three standard keyboard modifiers */
#define PW_SHIFT   1
#define PW_CTRL    2
#define PW_ALT     4

/* Whether a button has been pressed (DOWN) or released (UP) */
#define PW_DOWN    0
#define PW_UP      1

/* Mouse Buttons */
#define PW_LEFT_BUTTON   0
#define PW_MIDDLE_BUTTON 1
#define PW_RIGHT_BUTTON  2

#endif

