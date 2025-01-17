/*
  Map_trace
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

#ifndef DETAILED_DEM_H
#define DETAILED_DEM_H

#include <vector>
#include <iostream>
#include <string>

class Detailed_dem
{
public:
  // size is the size of the array - e.g. 257 for 257x257
  //
  // xmin, xmax, ymin, ymax are the coords of the corners.
  //
  // resolution is the resolution of the height data - e.g. 0.1 for
  // 0.1m accuracy. More accuracy means that a smaller range of
  // heights can be stored.
  Detailed_dem(unsigned int size, 
               float xmin, float ymin, float xmax, float ymax,
               float resolution);
  
  float get_height(int i, int j) const
    {
      long int val = ( (int) (m_vals[calc_index(i, j)] & 0x7fffffff)) -
        (int) m_zero_offset;
      
      return val * m_resolution;
    }
  
  void draw_line(float x0, float y0,
                 float x1, float y1,
                 float height);

  void draw_ridge_line(float x0, float y0,
                       float x1, float y1,
                       float height, float width2,  // width2 is half-width
                       int id); // different for each ridge
  
  // diffuses all the sticky values out to fill in the non-sticky values.
  void diffuse(float tol);
  
  // uses the reasonably accurate wind field to linearly interpolate
  // the original lines.
  void recalc_heights();
  
  void do_output(
    const std::string & tiff_file,
    const std::string & contour_file,
    const std::string & orog_file,
    int smooth_factor) const;
  
  void get_min_max_heights(float & hmin, float & hmax) const;
  
  void show() const;
  
  // assume that an unsigned int is 4 bytes. This should be private,
  // but MSVC++ is crap
  typedef unsigned int Val;
private:
  float m_xmin, m_xmax, m_ymin, m_ymax;
  unsigned m_size;
  float m_resolution;
  
  
  std::vector<Val> m_vals;

  struct Line 
  {
    float x0, y0, x1, y1, h, len;
  };
  
  std::vector<Line> m_lines;
  
  struct Ridge_line 
  {
    float x0, y0, x1, y1, h, w;
    int id;
    float len;
  };
  
  std::vector<Ridge_line> m_ridge_lines;
  
  Val m_zero_offset;
  
  // get approximate value
  void do_initial_pass();
  void add_ridges();
  
  enum Dir {LEFT, UP, RIGHT, DOWN};
  void get_val_dist(int i0, int j0, float & val, int & dist, Dir dir);
  
  unsigned calc_index(int i, int j) const
    {
      if (i < 0) i = -i;
      else if (i >= (int) m_size) i = (m_size-2) - (i - m_size);
      if (j < 0) j = -j;
      else if (j >= (int) m_size) j = (m_size-2) - (j - m_size);;
      
      return i + j * m_size;
    }
  
  // also clears the sticky bit
  void set_height(int i, int j, float val)
    {
      Val v = ( Val ) ( (int) (val / m_resolution) + 
                        (int) m_zero_offset );
      m_vals[calc_index(i, j)] = v;
    }
  
  bool get_sticky(int i, int j) const
    {
      return ( ( m_vals[calc_index(i, j)] >> 31 ) != 0 );
    }
  
  void set_sticky(int i, int j)
    {
      m_vals[calc_index(i, j)] |= 0x80000000;
    }
  void clear_sticky(int i, int j)
    {
      m_vals[calc_index(i, j)] |= 0x7fffffff;
    }
  
  
  
};

#endif

