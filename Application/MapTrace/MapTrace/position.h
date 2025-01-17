#ifndef POSITION_H
#define POSITION_H

#include <cmath>

struct Position
{
  Position(float x, float y) : x(x), y(y) {}
  bool operator==(const Position & rhs) 
    {return ((rhs.x == x) && (rhs.y == y));}
  bool operator!=(const Position & rhs) 
    {return ((rhs.x != x) || (rhs.y != y));}
  float dist(const Position & rhs) const 
    {return (float) hypot(rhs.x - x, rhs.y - y);}
  
  void clip() {
    x = x < 0 ? 0 : x ;
    y = y < 0 ? 0 : y ;
    x = x > 1.0F ? 1.0F : x ;
    y = y > 1.0F ? 1.0F : y ; 
  }
  float x, y;
};


#endif
