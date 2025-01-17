#ifndef STRING_H
#define STRING_H

#include "RenderManager.h"
#include "RenderObject.h"

class Rope : public RenderObject
{
public:
  typedef std::vector<Vector3> Points;

  Rope();
  ~Rope();

  Points& GetPoints() {return mPoints;}

  void SetColour(const Vector4& colour) {mColour = colour;}

  virtual void RenderUpdate(class Viewport* viewport, int renderLevel);
private:
  Points mPoints;
  Vector4 mColour;
};
#endif

