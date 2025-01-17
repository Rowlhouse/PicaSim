/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef CONTOUR_COLLECTION_H
#define CONTOUR_COLLECTION_H

#include "contour.h"

#include <vector>
#include <iosfwd>

class Detailed_dem;

class Contour_collection
{
public:
  Contour_collection() {};

  void add_contour(const Contour & contour) {m_contours.push_back(contour);}

  void draw(float current_level) const;
  
  void insert_in_dem(Detailed_dem & dem) const;

  // delete all contours within tol of level
  void delete_contours(float level, float tol);
  
  void save(std::ofstream & out_file) const;
  void load(std::ifstream & in_file);

private:
  typedef std::vector<Contour>::iterator Contour_it;
  typedef std::vector<Contour>::const_iterator Contour_const_it;
  std::vector<Contour> m_contours;
};


#endif
