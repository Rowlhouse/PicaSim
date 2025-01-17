#include "SkyGrid.h"
// #include "RenderManager.h"
// #include "Trace.h"
// #include "Graphics.h"
// #include "ShaderManager.h"
// #include "Shaders.h"

// #include <IwGx.h>
// #include <IwGxFont.h>

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::Init()
{
  TRACE_FUNCTION_ONLY(1);
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);
  mType = SPHERE;
  mDistance = -1.0f;
}

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::Terminate()
{
  TRACE_FUNCTION_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);
}

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::RenderUpdate(class Viewport* viewport, int renderLevel)
{
  TRACE_METHOD_ONLY(2);
  if (!mEnable)
    return;

  if (mLinePoints.empty() && mLineStrips.empty())
    return;

  DisableFog disableFog;
  DisableDepthTest disableDepthTest;
  DisableDepthMask disableDepthMask;

  const SimpleShader* simpleShader = (SimpleShader*) ShaderManager::GetInstance().GetShader(SHADER_SIMPLE);
  if (gGLVersion == 2)
    simpleShader->Use();

  esPushMatrix();

  GLMat44 glTM;
  ConvertTransformToGLMat44(mTM, glTM);
  esMultMatrixf(&glTM[0][0]);

  esSetModelViewProjectionMatrix(simpleShader->u_mvpMatrix);

  // First the lines
  size_t numLinePts = mLinePoints.size();
  if (numLinePts)
  {
    if (gGLVersion == 1)
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, &mLinePoints[0].x);
      glColorPointer(4, GL_FLOAT, 0, &mLinePointColours[0].x);
    }
    else
    {
      glEnableVertexAttribArray(simpleShader->a_position);
      glEnableVertexAttribArray(simpleShader->a_colour);
      glVertexAttribPointer(simpleShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mLinePoints[0].x);
      glVertexAttribPointer(simpleShader->a_colour, 4, GL_FLOAT, GL_FALSE, 0, &mLinePointColours[0].x);
    }
    glDrawArrays(GL_LINES, 0, numLinePts);
  }

  // Now the lines strips
  for (size_t iStrip = 0 ; iStrip != mLineStrips.size() ; ++iStrip)
  {
    const LineStrip& linestrip = mLineStrips[iStrip];
    size_t numPts = linestrip.size();
    if (numPts)
    {
      if (gGLVersion == 1)
      {
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &linestrip[0].x);
        glColor4f(mLineStripColours[iStrip].x, mLineStripColours[iStrip].y, mLineStripColours[iStrip].z, mLineStripColours[iStrip].w);
      }
      else
      {
        glEnableVertexAttribArray(simpleShader->a_position);
        glDisableVertexAttribArray(simpleShader->a_colour);
        glVertexAttribPointer(simpleShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &linestrip[0].x);
        glVertexAttrib4fv(simpleShader->a_colour, &mLineStripColours[iStrip].x);
      }
      glDrawArrays(GL_LINE_STRIP, 0, linestrip.size());
    }
  }

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }

  esPopMatrix();
}

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::AddCircle(const Vector3& centre, const Vector3& axis, float radius, const Vector4& colour, int divisions)
{
  Vector3 radial1;
  if (fabsf(axis.Dot(Vector3(0,0,1))) < 0.5f)
    radial1 = axis.Cross(Vector3(0,0,1));
  else
    radial1 = axis.Cross(Vector3(0,1,0));
  radial1.Normalise();
  radial1 *= radius;
  Vector3 radial2 = radial1.Cross(axis);

  Vector3 prevPos = centre + radial2;

  mLineStripColours.push_back(colour);

  mLineStrips.push_back(LineStrip());
  LineStrip& linestrip = mLineStrips.back(); 

  linestrip.push_back(prevPos);

  for (int i = 0 ; i != divisions ; ++i)
  {
    float a = (2.0f * PI * (i+1)) / divisions;
    float s = FastSin(a);
    float c = FastCos(a);
    Vector3 pos = centre + radial1 * s + radial2 * c;
    linestrip.push_back(pos);
    prevPos = pos;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::SetSphere(float radius)
{
  if (radius == mDistance && mType == SPHERE)
    return;
  mDistance = radius;
  mType = SPHERE;

  mLinePoints.clear();
  mLinePointColours.clear();
  mLineStrips.clear();

  const int numAzimuths = 8;
  const int numElevations = 4;
  float azimuths[numAzimuths];
  float elevations[numElevations+1];

  for (int i = 0 ; i != numAzimuths ; ++i)
    azimuths[i] = TWO_PI * float(i) / numAzimuths;
  for (int j = 0 ; j != numElevations+1 ; ++j)
    elevations[j] = HALF_PI * float(j) / numElevations;

  Vector4 col[2] = {Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.5f, 0.5f, 0.5f, 1.0f)};
  for (int j = 0 ; j != numElevations ; ++j)
  {
    Vector3 pos(0,0,0);
    pos.z += radius * FastSin(elevations[j]);
    float r = radius * FastCos(elevations[j]);
    AddCircle(pos, Vector3(0,0,1), r, col[j%2], 32);
  }

  for (int i = 0 ; i != numAzimuths ; ++i)
  {
    float ca = FastCos(azimuths[i]);
    float sa = FastSin(azimuths[i]);
    Vector3 prevPos(radius * ca, radius * sa, 0.0f);
    mLineStripColours.push_back(col[i%2]);
    mLineStrips.push_back(LineStrip());
    LineStrip& linestrip = mLineStrips.back();
    linestrip.push_back(prevPos);
    for (int j = 1 ; j != numElevations+1 ; ++j)
    {
      float r = radius * FastCos(elevations[j]);
      float z = radius * FastSin(elevations[j]);
      float x = r * ca;
      float y = r * sa;
      Vector3 pos(x, y, z);
      linestrip.push_back(pos);
      prevPos = pos;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void SkyGrid::SetBox(float distance)
{
  if (distance == mDistance && mType == BOX)
    return;

  mDistance = distance;
  mType = BOX;

  mLinePoints.clear();
  mLinePointColours.clear();
  mLineStrips.clear();

  const int numPerSide = 9; // should be odd
  const int numHeights = 9;

  const float dist1 = distance * FastCos(DegreesToRadians(30.0f));
  const float dist2 = distance * FastSin(DegreesToRadians(30.0f));
  const float height = distance * FastSin(DegreesToRadians(60.0f));

  Vector4 col[2] = {Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.5f, 0.5f, 0.5f, 1.0f)};

  Vector3 bottoms[4];
  Vector3 tops[4];

  Vector3 alongDir(0.0f, 1.0f, 0.0f);
  Vector3 acrossDir(1.0f, 0.0f, 0.0f);

  bottoms[0] = alongDir * dist1 + acrossDir * dist2;
  bottoms[1] = -alongDir * dist1 + acrossDir * dist2;
  bottoms[2] = alongDir * dist1 - acrossDir * dist2;
  bottoms[3] = -alongDir * dist1 - acrossDir * dist2;
  for (int s = 0 ; s != 4 ; ++s)
    tops[s] = bottoms[s] + Vector3(0,0,height);

  for (int s = 0 ; s != 4 ; s += 2)
  {
    Vector3 b1 = bottoms[s];
    Vector3 t1 = tops[s];
    Vector3 b2 = bottoms[(s+1)%4];
    Vector3 t2 = tops[(s+1)%4];

    for (int i = 0 ; i != numPerSide ; ++i)
    {
      float fi = numPerSide > 1 ? float(i) / (numPerSide - 1.0f) : 0.0f;
      Vector3 b = b1 + (b2 - b1) * fi;
      Vector3 t = t1 + (t2 - t1) * fi;
      mLinePoints.push_back(b);
      mLinePoints.push_back(t);
      Vector4 c = col[i == 0 || i+1 == numPerSide || i == numPerSide/2 ? 0 : 1];
      mLinePointColours.push_back(c);
      mLinePointColours.push_back(c);
    }

    for (int j = 0 ; j != numHeights ; ++j)
    {
      float fj = numHeights > 1 ? float(j) / (numHeights - 1.0f) : 0.0f;
      Vector3 p1 = b1 + (t1 - b1) * fj;
      Vector3 p2 = b2 + (t2 - b2) * fj;
      mLinePoints.push_back(p1);
      mLinePoints.push_back(p2);
      Vector4 c = col[j%2];
      mLinePointColours.push_back(c);
      mLinePointColours.push_back(c);
    }
  }
}
