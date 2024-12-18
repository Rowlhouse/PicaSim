#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "RenderObject.h"
#include "RenderOverlayObject.h"
#include "RenderManager.h"
#include "Helpers.h"

#include <vector>
#include <string>

class DebugRenderer : public RenderObject, RenderOverlayObject, RenderGxObject
{
public:
  void Init();
  void Terminate();

  // 3D drawing
  void DrawPoint(const Vector3& pos, float size, const Vector3& colour);
  void DrawPoint(const Transform& tm, float size, const Vector3& colour);
  void DrawLine(const Vector3& from, const Vector3& to, const Vector3& colour);
  void DrawArrow(const Vector3& from, const Vector3& to, const Vector3& colour);
  void DrawCircle(const Vector3& centre, const Vector3& axis, float radius, const Vector3& colour, int divisions = 12);
  void DrawVector(const Vector3& from, const Vector3& delta, const Vector3& colour);
  void DrawBox(const Transform& centreTM, const Vector3& extents);

  // 2D drawing
  void DrawLine2D(const Vector2& from, const Vector2& to, const Vector3& colour);
  void DrawPoint2D(const Vector2& pos, float size, const Vector3& colour);
  void DrawText2D(const std::string& text, const Vector2& pos, const Vector3& colour);

  // Graphs
  void SetGraphProperties(uint graphID, size_t numPts, float minVal, float maxVal, Vector3 colour);
  void DisableGraph(uint graphID);
  void AddGraphPoint(uint graphID, float val);

private:
  struct Point
  {
    Point(const Vector3& pos, float size, const Vector3& colour) : mSize(size), mColour(colour) {mTM.SetIdentity(); mTM.SetTrans(pos);}
    Point(const Transform& tm, float size, const Vector3& colour) : mTM(tm), mSize(size), mColour(colour) {}
    Transform mTM;
    Vector3 mColour;
    float mSize;
  };
  typedef std::vector<Point> Points; 

  struct Line
  {
    Line(const Vector3& start, const Vector3& end, const Vector3& colour) : mStart(start), mEnd(end), mColour(colour) {}
    Vector3 mStart;
    Vector3 mEnd;
    Vector3 mColour;
  };
  typedef std::vector<Line> Lines;

  struct LinePoint2D
  {
    LinePoint2D(const Vector2& pos, const Vector3& colour) : mPos(pos), mColour(colour) {}
    Vector2 mPos;
    Vector3 mColour;
  };
  typedef std::vector<LinePoint2D> LinePoints2D;

  struct Text2D
  {
    Text2D(const std::string& text, const Vector2& pos, const Vector3& colour) : mText(text), mPos(pos), mColour(colour) {}
    std::string mText;
    Vector2 mPos;
    Vector3 mColour;
  };
  typedef std::vector<Text2D> Texts2D;

  struct GraphPoint
  {
    GraphPoint(float x, float y) : mX(x), mY(y) {}
    float mX, mY;
  };

  struct Graph
  {
    typedef std::vector<GraphPoint> GraphPoints;
    GraphPoints mGraphPoints;
    float mGraphMinVal, mGraphValRange;
    Vector3 mColour;
  };

  void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;
  void RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;
  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  Points mPoints;
  Lines mLines;
  LinePoints2D mLinePoints2D;
  Texts2D mTexts2D;
  static const uint MAX_NUM_GRAPHS = 8;
  Graph mGraphs[MAX_NUM_GRAPHS];
};

#endif
