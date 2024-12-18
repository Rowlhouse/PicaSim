/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

#include "text_overlay.h"
#ifdef WITH_PLIB
#include <plib/fnt.h>
#endif
#include <GL/glut.h>
#include <stdio.h>

int Text_overlay::add_entry(int x, int y, const char * text)
{
  if (entries.size() >= (MAX_ENTRIES-1))
  {
    printf("Can't overlay text %s\n", text);
    return -1;
  }
  entries.push_back(Entry(text, x, y));
  return 0;
}

void Text_overlay::display()
{
  if (entries.empty())
    return;

#ifdef WITH_PLIB
  // set up projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 100, 0, 100);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable      ( GL_LIGHTING   ) ;
  glDisable      ( GL_FOG        ) ;
  glDisable      ( GL_DEPTH_TEST ) ;
  glDisable      ( GL_CULL_FACE  ) ;
  glEnable       ( GL_ALPHA_TEST ) ;
  glEnable       ( GL_BLEND ) ;
  glAlphaFunc    ( GL_GREATER, 0.1f ) ;
  glBlendFunc    ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);

  glColor4f(0, 0, 0, 1);

  // prepare fonts

  static fntRenderer * texout;
  static fntTexFont * font;
  static bool init = false;
  if (init == false)
  {
    init = true;
    texout = new fntRenderer;
    font = new fntTexFont("font.txf") ;
  }

  texout->setFont(font);
  texout->setPointSize(3);

  texout->begin () ;
  
  // do text

  unsigned int i;
  for (i = 0 ; i < entries.size() ; i++)
  {
    texout->start2f ( entries[i].x, entries[i].y ) ;
    texout->puts(entries[i].text.c_str());
  }

  texout->end () ;
 
  // undo stuff
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();

#else

  // set up projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  
  glLoadIdentity();
  gluOrtho2D(0, 100, 0, 100);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  
  // output text
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  
  glColor3f(0, 0, 0);
  unsigned int i, j;
  for (i = 0 ; i < entries.size() ; i++)
  {
    glRasterPos2i(entries[i].x, entries[i].y);
    unsigned int len = strlen(entries[i].text.c_str());
    for (j = 0 ; j < len ; ++j)
    {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, entries[i].text[j]);
    }
  }
  // undo
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();
#endif
}  





