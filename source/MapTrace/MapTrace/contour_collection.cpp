/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#include "contour_collection.h"
#include "detailed_dem.h"

#include <GL/glut.h>

#include <fstream>
#include <algorithm>
using namespace std;

void Contour_collection::draw(float current_level) const
{
  for (Contour_const_it it = m_contours.begin() ; 
       it != m_contours.end() ; 
       ++it)
  {
    glPushMatrix();
    it->draw(current_level);
    glPopMatrix();
  }
  
}

void Contour_collection::insert_in_dem(Detailed_dem & dem) const
{
  for (Contour_const_it it = m_contours.begin() ; 
       it != m_contours.end() ; 
       ++it)
  {
    it->insert_in_dem(dem);
  }
}

struct Approx
{
  Approx(float level, float tol) : m_level(level), m_tol(tol) {};
  bool operator()(const Contour & con) const 
    {return con.approx(m_level, m_tol);}
  const float m_level, m_tol;
};

    
void Contour_collection::delete_contours(float level, float tol)
{
  Contour_it pos = remove_if(m_contours.begin(),
                             m_contours.end(),
                             Approx(level, tol));
  m_contours.erase(pos, m_contours.end());
}

void Contour_collection::save(ofstream & out_file) const
{
  out_file << m_contours.size() << endl;
  
  for (Contour_const_it it = m_contours.begin() ; 
       it != m_contours.end() ; 
       ++it)
  {
    it->save(out_file);
  }
}

void Contour_collection::load(ifstream & in_file)
{
  // clear out what we have
  m_contours.clear();
  
  unsigned int num, i;
  
  in_file >> num;
  cout << "loading " << num << " contours\n";
  
  for (i = 0 ; i < num ; ++i)
  {
    Contour contour;
    contour.load(in_file);
    m_contours.push_back(contour);
  }
}

