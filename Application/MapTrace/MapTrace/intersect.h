#ifndef INTERSECT_H
#define INTERSECT_H

#ifndef INTERSECT_TOL
#define INTERSECT_TOL (1.0e-7)
#endif

#include <stdio.h>
#include <math.h>

/// Calculates the intersection of two lines. Line A is defined by R =
/// A0 + ta * (A1 - A0) with A0 = (a0x, a0y) etc.
///
/// If false is returned the lines are parallel
///
/// If true is returned, the lines intersect and the two parameters
/// are returned.
inline bool intersect(const float a0x, const float a0y, 
                      const float a1x, const float a1y, 
                      const float b0x, const float b0y, 
                      const float b1x, const float b1y, 
                      float & ta,
                      float & tb)
{
  const float denom = (b1x - b0x) * (a1y - a0y) - (b1y - b0y) * (a1x - a0x);
  
  if (fabs(denom) < INTERSECT_TOL)
  {
    return false;
  }
  
  const float a0x_b0x = a0x - b0x;
  const float a0y_b0y = a0y - b0y;
  const float inv_denom = 1.0f/denom;

  ta = ( (b1y - b0y) * a0x_b0x - (b1x - b0x) * a0y_b0y ) * inv_denom;
  tb = ( (a1y - a0y) * a0x_b0x - (a1x - a0x) * a0y_b0y ) * inv_denom;
  
  return true;
}


#endif
