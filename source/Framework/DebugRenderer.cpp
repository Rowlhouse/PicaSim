#include "DebugRenderer.h"
#include "RenderManager.h"
#include "Trace.h"
#include "Graphics.h"
#include "ShaderManager.h"
#include "Shaders.h"
#include "Viewport.h"

#include <IwGx.h>
#include <IwGxFont.h>

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::Init()
{
  TRACE_FUNCTION_ONLY(1);
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_DEBUG);
  RenderManager::GetInstance().RegisterRenderOverlayObject(this, 1);
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::Terminate()
{
  TRACE_FUNCTION_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_DEBUG);
  RenderManager::GetInstance().UnregisterRenderOverlayObject(this, 1);
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
}

static const float d = 0.5f;
static GLfloat pts[] = {
  -d,  0,  0,
   d,  0,  0, 
   0, -d,  0, 
   0,  d,  0, 
   0,  0, -d, 
   0,  0,  d
};

static GLfloat linePts[] = {
  0,0,0,
  0,0,0
};

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::SetGraphProperties(uint graphID, size_t numPts, float minVal, float maxVal, Vector3 colour)
{
  IwAssert(ROWLHOUSE, graphID < MAX_NUM_GRAPHS);
  Graph& graph = mGraphs[graphID];
  graph.mGraphMinVal = minVal;
  graph.mGraphValRange = maxVal - minVal;
  graph.mColour = colour;
  if (numPts == graph.mGraphPoints.size())
    return;
  graph.mGraphPoints.resize(numPts, GraphPoint(0,0));
  for (size_t i = 0 ; i != numPts ; ++i)
    graph.mGraphPoints[i].mX = float(i) / (numPts-1.0f);
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DisableGraph(uint graphID)
{
  IwAssert(ROWLHOUSE, graphID < MAX_NUM_GRAPHS);
  Graph& graph = mGraphs[graphID];
  graph.mGraphPoints.resize(0, GraphPoint(0,0));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::AddGraphPoint(uint graphID, float val)
{
  IwAssert(ROWLHOUSE, graphID < MAX_NUM_GRAPHS);
  Graph& graph = mGraphs[graphID];
  if (graph.mGraphPoints.empty())
    return;

  size_t numPts = graph.mGraphPoints.size();
  for (size_t i = 1 ; i != numPts ; ++i)
    graph.mGraphPoints[i-1].mY = graph.mGraphPoints[i].mY;

  graph.mGraphPoints[numPts-1].mY = (val - graph.mGraphMinVal) / graph.mGraphValRange;
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig)
{
  DisableDepthMask disableDepthMask;
  DisableDepthTest disableDepthTest;

  const SimpleShader* simpleShader = (SimpleShader*) ShaderManager::GetInstance().GetShader(SHADER_SIMPLE);
  const ControllerShader* controllerShader = (ControllerShader*) ShaderManager::GetInstance().GetShader(SHADER_CONTROLLER);

  esPushMatrix();
  esLoadIdentity();
  esTranslatef(float(displayConfig.mLeft), float(displayConfig.mBottom), 0.0f);
  esScalef(float(displayConfig.mWidth), float(displayConfig.mHeight), 1.0f);

  // Graphs
  {
    if (gGLVersion == 1)
    {
      glEnableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
      controllerShader->Use();
      glEnableVertexAttribArray(controllerShader->a_position);
      esSetModelViewProjectionMatrix(controllerShader->u_mvpMatrix);
    }

    for (uint iGraph = 0 ; iGraph != MAX_NUM_GRAPHS ; ++iGraph)
    {
      Graph& graph = mGraphs[iGraph];
      if (graph.mGraphPoints.empty())
        continue;

      if (gGLVersion == 1)
      {
        glColor4f(graph.mColour.x,graph.mColour.y,graph.mColour.z,1.0f);
        glVertexPointer(2, GL_FLOAT, 0, &graph.mGraphPoints[0].mX);
      }
      else
      {
        glUniform4f(controllerShader->u_colour, graph.mColour.x, graph.mColour.y, graph.mColour.z, 1.0f);
        glVertexAttribPointer(controllerShader->a_position, 2, GL_FLOAT, GL_FALSE, 0, &graph.mGraphPoints[0].mX);
      }
      glDrawArrays(GL_LINE_STRIP, 0, graph.mGraphPoints.size());
    }
  }

  // General 2D lines
  if (!mLinePoints2D.empty())
  {
    if (gGLVersion == 1)
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
    }
    else
    {
      simpleShader->Use();
    }

    esSetModelViewProjectionMatrix(simpleShader->u_mvpMatrix);

    if (gGLVersion == 1)
    {
      glVertexPointer(2, GL_FLOAT, sizeof(LinePoint2D), &mLinePoints2D[0].mPos);
      glColorPointer(4, GL_FLOAT, sizeof(LinePoint2D), &mLinePoints2D[0].mColour);
    }
    else
    {
      glVertexAttribPointer(simpleShader->a_position, 2, GL_FLOAT, GL_FALSE, sizeof(LinePoint2D), &mLinePoints2D[0].mPos);
      glVertexAttribPointer(simpleShader->a_colour, 2, GL_FLOAT, GL_FALSE, sizeof(LinePoint2D), &mLinePoints2D[0].mColour);
    }
    glDrawArrays(GL_LINES, 0, mLinePoints2D.size());
    mLinePoints2D.clear();
  }

  esPopMatrix();
  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::RenderUpdate(class Viewport* viewport, int renderLevel)
{
  TRACE_METHOD_ONLY(2);
  const SimpleShader* simpleShader = (SimpleShader*) ShaderManager::GetInstance().GetShader(SHADER_SIMPLE);

  if (gGLVersion == 1)
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, pts);
  }
  else
  {
    simpleShader->Use();

    glVertexAttribPointer(simpleShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, pts);
    glEnableVertexAttribArray(simpleShader->a_position);

    glDisableVertexAttribArray(simpleShader->a_colour);
  }

  for (Points::iterator it = mPoints.begin() ; it != mPoints.end() ; ++it)
  {
    const Point& point = *it;

    esPushMatrix();
    GLMat44 glTM;
    ConvertTransformToGLMat44(point.mTM, glTM);
    esMultMatrixf(&glTM[0][0]);
    esScalef(point.mSize, point.mSize, point.mSize);
    if (gGLVersion == 1)
      glColor4f(point.mColour.x,point.mColour.y,point.mColour.z,1);
    else
      glVertexAttrib3fv(simpleShader->a_colour, &point.mColour.x);
    esSetModelViewProjectionMatrix(simpleShader->u_mvpMatrix);
    glDrawArrays(GL_LINES, 0, 6);
    esPopMatrix();
  }
  mPoints.clear();

  esSetModelViewProjectionMatrix(simpleShader->u_mvpMatrix);

  for (Lines::iterator it = mLines.begin() ; it != mLines.end() ; ++it)
  {
    const Line& line = *it;
    linePts[0] = line.mStart.x;
    linePts[1] = line.mStart.y;
    linePts[2] = line.mStart.z;
    linePts[3] = line.mEnd.x;
    linePts[4] = line.mEnd.y;
    linePts[5] = line.mEnd.z;

    if (gGLVersion == 1)
    {
      glVertexPointer(3, GL_FLOAT, 0, linePts);
      glColor4f(line.mColour.x,line.mColour.y,line.mColour.z,1);
    }
    else
    {
      glVertexAttribPointer(simpleShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, linePts);
      glVertexAttrib3fv(simpleShader->a_colour, &line.mColour.x);
    }
    glDrawArrays(GL_LINES, 0, 2);
  }
  mLines.clear();

  if (gGLVersion == 1)
    glDisableClientState(GL_VERTEX_ARRAY);
}

//----------------------------------------------------------------------------------------------------------------------
void DebugRenderer::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
  TRACE_METHOD_ONLY(2);
  if (mTexts2D.empty())
    return;

  CIwGxFont* font = (CIwGxFont*) IwGetResManager()->GetResNamed("normal", IW_GX_RESTYPE_FONT);
  IwGxFontSetFont(font);

  Colour origColour = IwGxFontGetCol();

  IwGxLightingOn();
  uint16 fontHeight = font->GetHeight();

  for (Texts2D::iterator it = mTexts2D.begin() ; it != mTexts2D.end() ; ++it)
  {
    const Text2D& text2D = *it;
    IwGxFontSetRect(CIwRect(
      (int16) (displayConfig.mLeft + text2D.mPos.x * displayConfig.mWidth),
      (int16) (displayConfig.mBottom + text2D.mPos.y * displayConfig.mHeight), 
      (int16) displayConfig.mWidth, fontHeight));
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_LEFT);
    int r = ClampToRange((int) (text2D.mColour.x * 255), 0, 255);
    int g = ClampToRange((int) (text2D.mColour.y * 255), 0, 255);
    int b = ClampToRange((int) (text2D.mColour.z * 255), 0, 255);
    int a = 255;
    uint32 col = (r << 0) + (g << 8) + (b << 16) + (a << 24);
    IwGxFontSetCol(col);
    IwGxFontDrawText(text2D.mText.c_str());
  }
  IwGxFontSetCol(origColour);
  IwGxLightingOff();

  mTexts2D.clear();
}


//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawPoint(const Vector3& pos, float size, const Vector3& colour)
{
  mPoints.push_back(Point(pos, size, colour));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawPoint(const Transform& tm, float size, const Vector3& colour)
{
  mPoints.push_back(Point(tm, size, colour));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawLine(const Vector3& from, const Vector3& to, const Vector3& colour)
{
  mLines.push_back(Line(from, to, colour));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawArrow(const Vector3& from, const Vector3& to, const Vector3& colour)
{
  mLines.push_back(Line(from, to, colour));
  Vector3 dir = to - from;
  float length = dir.GetLength();
  dir *= (1.0f/length);

  float headLength = length * 0.2f;
  float headWidth = length * 0.1f;

  Vector3 sideDir;
  if (fabsf(dir.Dot(Vector3(0, 0, 1))) < 0.5f)
    sideDir = dir.Cross(Vector3(0,0,1)).GetNormalised();
  else
    sideDir = dir.Cross(Vector3(0,1,0)).GetNormalised();
  Vector3 upDir = sideDir.Cross(dir);

  mLines.push_back(Line(to, to - dir * headLength + sideDir * headWidth, colour));
  mLines.push_back(Line(to, to - dir * headLength - sideDir * headWidth, colour));
  mLines.push_back(Line(to, to - dir * headLength + upDir * headWidth, colour));
  mLines.push_back(Line(to, to - dir * headLength - upDir * headWidth, colour));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawCircle(const Vector3& centre, const Vector3& axis, float radius, const Vector3& colour, int divisions)
{
  Vector3 radial1;
  if (fabsf(axis.Dot(Vector3(0,0,1))) < 0.5f)
    radial1 = axis.Cross(Vector3(0,0,1));
  else
    radial1 = axis.Cross(Vector3(0,1,0));
  radial1.Normalise();
  radial1 *= radius;
  Vector3 radial2 = radial1.Cross(axis);

  Vector3 prevP = centre + radial2;
  for (int i = 0 ; i != divisions ; ++i)
  {
    float a = (2.0f * PI * (i+1)) / divisions;
    float s = FastSin(a);
    float c = FastCos(a);

    Vector3 p = centre + radial1 * s + radial2 * c;
    DrawLine(prevP, p, colour);
    prevP = p;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawVector(const Vector3& from, const Vector3& delta, const Vector3& colour)
{
  mLines.push_back(Line(from, from+delta, colour));
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawBox(const Transform& centreTM, const Vector3& extents)
{
  Vector3 he = extents * 0.5f;
  Vector3 X = centreTM.RowX() * extents.x;
  Vector3 Y = centreTM.RowY() * extents.y;
  Vector3 Z = centreTM.RowZ() * extents.z;
  Vector3 corner = centreTM.GetTrans() - centreTM.RotateVec(he);
  DrawVector(corner, X, Vector3(1, 0, 0));
  DrawVector(corner, Y, Vector3(0, 1, 0));
  DrawVector(corner, Z, Vector3(0, 0, 1));

  DrawVector(corner+Z, X, Vector3(1, 1, 1));
  DrawVector(corner+Z, Y, Vector3(1, 1, 1));
  DrawVector(corner+Z+Y, X, Vector3(1, 1, 1));
  DrawVector(corner+Y, X, Vector3(1, 1, 1));
  DrawVector(corner+Y, Z, Vector3(1, 1, 1));

  DrawVector(corner+X, Y, Vector3(1, 1, 1));
  DrawVector(corner+X, Z, Vector3(1, 1, 1));
  DrawVector(corner+X+Z, Y, Vector3(1, 1, 1));
  DrawVector(corner+X+Y, Z, Vector3(1, 1, 1));
}


//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawLine2D(const Vector2& from, const Vector2& to, const Vector3& colour)
{
  LinePoint2D a(from, colour);
  LinePoint2D b(to, colour);
  mLinePoints2D.push_back(a);
  mLinePoints2D.push_back(b);
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawPoint2D(const Vector2& pos, float size, const Vector3& colour)
{
  DrawLine2D(pos - Vector2(size, 0.0f), pos + Vector2(size, 0.0f), colour);
  DrawLine2D(pos - Vector2(0.0f, size), pos + Vector2(0.0f, size), colour);
}

//---------------------------------------------------------------------------------------------------------------------
void DebugRenderer::DrawText2D(const std::string& text, const Vector2& pos, const Vector3& colour)
{
  mTexts2D.push_back(Text2D(text, pos, colour));
}
