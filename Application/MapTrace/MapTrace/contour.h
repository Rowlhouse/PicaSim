/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef CONTOUR_H
#define CONTOUR_H

#include "position.h"

#include <vector>
#include <iosfwd>
#include <cmath>

class Detailed_dem;

class Contour
{
public:
  Contour(float level = 0) : m_level(level) {};

  bool approx(float level, float tol) const {return (fabs(level - m_level) < tol);}

  void set_level(float level) {m_level = level;}
  float get_level() const {return m_level;}
  bool add_point(const Position & pos, float d = -1.0) {
    if (m_points.empty()) 
    { 
      m_points.push_back(pos); return true;
    }
    else if (m_points.back().dist(pos) > d ) 
    {
      m_points.push_back(pos);
      return true;
    }
    else
      return false;
  }
  void pop_end() {if (!m_points.empty()) m_points.pop_back();}
  void close();
  
//  const std::vector<Position> & get_points() const {return m_points;}
  
  void draw(float current_level) const;

  void insert_in_dem(Detailed_dem & dem) const;
  void insert_in_dem_as_ridge(Detailed_dem & dem, float width) const;

  void save(std::ofstream & out_file) const;
  void load(std::ifstream & in_file);

private:
  float m_level;
  typedef std::vector<Position>::iterator Points_it;
  typedef std::vector<Position>::const_iterator Points_const_it;
  
  std::vector<Position> m_points;
};

#endif
