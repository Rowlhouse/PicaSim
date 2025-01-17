/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#include "contour.h"
#include "detailed_dem.h"

#include <cmath>
#include <GL/glut.h>

#include <fstream>
using namespace std;

void Contour::close()
{
  int s = m_points.size();
  if (s > 0)
  {
    if (m_points[0] != m_points[s-1])
    {
      add_point(m_points[0]);
    }
  }
}

void Contour::draw(float current_level) const
{
  if (approx(current_level, 0.01))
    glColor4f(1, 0, 0, 1);
  else
    glColor4f(0, 0, 0, 1);
  
  glBegin(GL_LINE_STRIP);
  
  for (Points_const_it it = m_points.begin() ;
       it != m_points.end();
       ++it)
  {
    glVertex2f(it->x, it->y);
  }

  glEnd();
}

void Contour::insert_in_dem(Detailed_dem & dem) const
{
  if (m_points.empty())
  {
    return;
  }
  else if (m_points.size() == 1)
  {
    dem.draw_line(m_points[0].x, 
                  m_points[0].y, 
                  m_points[0].x, 
                  m_points[0].y,
                  m_level);
    return;
  }

  unsigned int i;
  for (i = 1 ; i < m_points.size() ; ++i)
  {
    dem.draw_line(m_points[i-1].x, 
                  m_points[i-1].y, 
                  m_points[i].x, 
                  m_points[i].y,
                  m_level);  
  }
}

void Contour::insert_in_dem_as_ridge(Detailed_dem & dem, float width) const
{
  static int ind = 0;
  ++ind;
//  printf("contour h = %f, w = %f\n", m_level, width);
  
  if (m_points.empty())
  {
    return;
  }
  else if (m_points.size() == 1)
  {
    dem.draw_ridge_line(m_points[0].x, 
                        m_points[0].y, 
                        m_points[0].x, 
                        m_points[0].y,
                        m_level, width, ind);
    return;
  }

  unsigned int i;
  for (i = 1 ; i < m_points.size() ; ++i)
  {
    dem.draw_ridge_line(m_points[i-1].x, 
                  m_points[i-1].y, 
                  m_points[i].x, 
                  m_points[i].y,
                  m_level, width, ind);  
  }
}


void Contour::save(ofstream & out_file) const
{
  out_file << m_level << endl;
  out_file << m_points.size() << endl;
  
  for (Points_const_it it = m_points.begin() ; 
       it != m_points.end() ; 
       ++it)
  {
    out_file << it->x << " " << it->y << endl;
  }
}


void Contour::load(ifstream & in_file)
{
  // clear out what we have (probably nothing)
  m_points.clear();

  in_file >> m_level;
  unsigned int num, i;
  in_file >> num;
  
  cout << "Loading contour level " << m_level <<
    " with " << num << " points." << endl;
  
  for (i = 0 ; i < num ; ++i)
  {
    float x, y;
    in_file >> x >> y;
    m_points.push_back(Position(x, y));
  }
}

