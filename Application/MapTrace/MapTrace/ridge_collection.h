/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/
#ifndef RIDGE_COLLECTION_H
#define RIDGE_COLLECTION_H

#include "contour.h"

#include <vector>
#include <iosfwd>

class Detailed_dem;

class Ridge_collection
{
public:
  Ridge_collection() {};

  void add_ridge(const Contour & ridge) {m_ridges.push_back(ridge);}

  void draw(float current_level) const;
  
  void insert_in_dem(Detailed_dem & dem) const;

  // delete all ridges within tol of level
  void delete_ridges(float level, float tol);
  
  void save(std::ofstream & out_file) const;
  void load(std::ifstream & in_file);

private:
  typedef std::vector<Contour>::iterator Ridge_it;
  typedef std::vector<Contour>::const_iterator Ridge_const_it;
  std::vector<Contour> m_ridges;
};


#endif
