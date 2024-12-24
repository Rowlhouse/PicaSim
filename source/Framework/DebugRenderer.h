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
  void DrawPoint(const glm::vec3& pos, float size, const glm::vec3& colour);
  void DrawPoint(const glm::mat4& tm, float size, const glm::vec3& colour);
  void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& colour);
  void DrawArrow(const glm::vec3& from, const glm::vec3& to, const glm::vec3& colour);
  void DrawCircle(const glm::vec3& centre, const glm::vec3& axis, float radius, const glm::vec3& colour, int divisions = 12);
  void DrawVector(const glm::vec3& from, const glm::vec3& delta, const glm::vec3& colour);
  void DrawBox(const glm::mat4& centreTM, const glm::vec3& extents);

  // 2D drawing
  void DrawLine2D(const glm::vec2& from, const glm::vec2& to, const glm::vec3& colour);
  void DrawPoint2D(const glm::vec2& pos, float size, const glm::vec3& colour);
  void DrawText2D(const std::string& text, const glm::vec2& pos, const glm::vec3& colour);

  // Graphs
  void SetGraphProperties(uint graphID, size_t numPts, float minVal, float maxVal, glm::vec3 colour);
  void DisableGraph(uint graphID);
  void AddGraphPoint(uint graphID, float val);

private:
  struct Point
  {
    Point(const glm::vec3& pos, float size, const glm::vec3& colour) : mSize(size), mColour(colour) {glm::mat4 mTM = glm::mat4(1.0f); mTM = glm::translate(mTM, pos); }
    Point(const glm::mat4& tm, float size, const glm::vec3& colour) : mTM(tm), mSize(size), mColour(colour) {}
    glm::mat4 mTM;
    glm::vec3 mColour;
    float mSize;
  };
  typedef std::vector<Point> Points; 

  struct Line
  {
    Line(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour) : mStart(start), mEnd(end), mColour(colour) {}
    glm::vec3 mStart;
    glm::vec3 mEnd;
    glm::vec3 mColour;
  };
  typedef std::vector<Line> Lines;

  struct LinePoint2D
  {
    LinePoint2D(const glm::vec2& pos, const glm::vec3& colour) : mPos(pos), mColour(colour) {}
    glm::vec2 mPos;
    glm::vec3 mColour;
  };
  typedef std::vector<LinePoint2D> LinePoints2D;

  struct Text2D
  {
    Text2D(const std::string& text, const glm::vec2& pos, const glm::vec3& colour) : mText(text), mPos(pos), mColour(colour) {}
    std::string mText;
    glm::vec2 mPos;
    glm::vec3 mColour;
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
    glm::vec3 mColour;
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
