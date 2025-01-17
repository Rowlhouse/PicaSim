/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#include "ridge_collection.h"
#include "detailed_dem.h"

#include <GL/glut.h>

#include <fstream>
#include <algorithm>
using namespace std;

void Ridge_collection::draw(float current_level) const
{
  for (Ridge_const_it it = m_ridges.begin() ; 
       it != m_ridges.end() ; 
       ++it)
  {
    glPushMatrix();
    it->draw(current_level);
    glPopMatrix();
  }
  
}

void Ridge_collection::insert_in_dem(Detailed_dem & dem) const
{
  for (Ridge_const_it it = m_ridges.begin() ; 
       it != m_ridges.end() ; 
       ++it)
  {
    it->insert_in_dem_as_ridge(dem, fabs(1.0f * it->get_level()));
  }
}

struct Approx
{
  Approx(float level, float tol) : m_level(level), m_tol(tol) {};
  bool operator()(const Contour & con) const 
    {return con.approx(m_level, m_tol);}
  const float m_level, m_tol;
};

    
void Ridge_collection::delete_ridges(float level, float tol)
{
  Ridge_it pos = remove_if(m_ridges.begin(),
                             m_ridges.end(),
                             Approx(level, tol));
  m_ridges.erase(pos, m_ridges.end());
}

void Ridge_collection::save(ofstream & out_file) const
{
  out_file << m_ridges.size() << endl;
  
  for (Ridge_const_it it = m_ridges.begin() ; 
       it != m_ridges.end() ; 
       ++it)
  {
    it->save(out_file);
  }
}

void Ridge_collection::load(ifstream & in_file)
{
  // clear out what we have
  m_ridges.clear();
  
  unsigned int num, i;
  
  in_file >> num;
  cout << "loading " << num << " ridges\n";
  
  for (i = 0 ; i < num ; ++i)
  {
    Contour ridge;
    ridge.load(in_file);
    m_ridges.push_back(ridge);
  }
}

