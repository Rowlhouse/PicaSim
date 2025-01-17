#include "Runway.h"

// #include "PicaSim.h"
// #include "ShaderManager.h"
// #include "Shaders.h"
// #include "AeroplanePhysics.h"

// #include <IwGL.h>
// #include <IwGx.h>

// #include <cmath>

//---------------------------------------------------------------------------------------------------------------------
Runway::Runway(const Transform& tm, float length, float width, const char* textureFile, Mode mode)
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().RegisterRenderObject(this, RENDER_LEVEL_OBJECTS);

  const GameSettings& gs = PicaSim::GetInstance().GetSettings();

  mLength = length;
  mWidth = width;
  mTM = tm;
  mMode = mode;

  if (textureFile)
  {
    LoadTextureFromFile(mTexture, textureFile);
    mTexture.SetClamping(false);
    mTexture.SetMipMapping(true);
    if (gs.mOptions.m16BitTextures && mTexture.GetWidth() > 512)
      mTexture.SetFormatHW(CIwImage::RGB_565);
    mTexture.Upload();
    TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", textureFile, mTexture.m_HWID);

    if (mTexture.GetFlags() & Texture::UPLOADED_F)
    {
      // Letting Marmalade handle the filtering/clamping doesn't seem to work... in particular clamping doesn't, so 
      // force it manually and re-upload
      glBindTexture(GL_TEXTURE_2D, mTexture.m_HWID);

      // filtering
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      mTexture.Upload();
      TRACE_FILE_IF(1) TRACE("Uploaded texture %s id %d", textureFile, mTexture.m_HWID);
    }
  }

  // Wait until the render update to create etc and then we can check to see what is necessary
}

//---------------------------------------------------------------------------------------------------------------------
void Runway::SetUpRunway()
{
  int numRows = Maximum((int) (mLength / Environment::GetInstance().GetTerrain().GetHeightfield().getCellSize()), 2);
  float dx = mLength / numRows;
  float dy = mWidth;
  float dUV = dx/dy;

  mPoints.resize(0);
  mUVs.resize(0);
  mColours.resize(0);

  size_t numPts = (numRows +2) * 10;
  mPoints.reserve(numPts);
  mUVs.reserve(numPts);
  mColours.reserve(numPts);

  Transform origin = mTM;

  Vector3 start = origin.GetTrans() + origin.RotateVec(-0.5f * Vector3(mLength, mWidth, 0.0f));
  Vector3 offset = origin.RotateVec(Vector3(0.0f, mWidth, 0.0f));
  float heightOffset = origin.GetTrans().z;

  const float frac = 0.1f;
  int index = 0;

  float endDist = 1.5f;

  float uv = -dUV * endDist/dx;
  MakeRow(start, origin, offset, -endDist, endDist, uv, dUV * endDist/dx, 0.0f, 1.0f, frac, heightOffset);
  uv = 0.0f;
  for (int i = 0 ; i != numRows ; ++i)
  {
    MakeRow(start, origin, offset, i*dx, dx, uv, dUV, 1.0f, 1.0f, frac, heightOffset);
    uv += dUV;
  }
  MakeRow(start, origin, offset, numRows * dx, endDist, uv, dUV * endDist/dx, 1.0f, 0.0f, frac, heightOffset);
}

//---------------------------------------------------------------------------------------------------------------------
void Runway::SetUpCircle()
{
  float circumference = 2.0f * PI * mLength;

  int numRows = 64;
  float dUV = circumference / (mWidth * numRows);

  mPoints.resize(0);
  mUVs.resize(0);
  mColours.resize(0);

  Vector3 xDir(1.0f, 0.0f, 0.0f);
  Vector3 yDir(0.0f, -1.0f, 0.0f);
  float innerRadius = mLength - mWidth * 0.5f;
  float outerRadius = innerRadius * mWidth;

  Vector3 centre = mTM.GetTrans();

  float frac = 0.1f;
  float f1 = frac;
  float f2 = 1.0f - frac;
  float heightOffset = mTM.GetTrans().z;

  float uv = 0.0f;

  for (int iRow = 0 ; iRow != numRows+1 ; ++iRow)
  {
    float angle1 = iRow * TWO_PI / numRows;
    float sinAngle1 = FastSin(angle1);
    float cosAngle1 = FastCos(angle1);

    float angle2 = (iRow + 1) * TWO_PI / numRows;
    float sinAngle2 = FastSin(angle2);
    float cosAngle2 = FastCos(angle2);

    Vector3 radialDir1 = xDir * cosAngle1 + yDir * sinAngle1; 
    Vector3 radialDir2 = xDir * cosAngle2 + yDir * sinAngle2; 

    Vector3 p0 = centre + radialDir1 * innerRadius;
    Vector3 p1 = centre + radialDir1 * (innerRadius + f1 * mWidth);
    Vector3 p2 = centre + radialDir1 * (innerRadius + f2 * mWidth);
    Vector3 p3 = centre + radialDir1 * (innerRadius +      mWidth);

    Vector3 q0 = centre + radialDir2 * innerRadius;
    Vector3 q1 = centre + radialDir2 * (innerRadius + f1 * mWidth);
    Vector3 q2 = centre + radialDir2 * (innerRadius + f2 * mWidth);
    Vector3 q3 = centre + radialDir2 * (innerRadius +      mWidth);

    Environment::GetInstance().GetTerrain().GetTerrainHeight(p0, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(p1, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(p2, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(p3, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(q0, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(q1, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(q2, true);
    Environment::GetInstance().GetTerrain().GetTerrainHeight(q3, true);
    p0.z += heightOffset;
    p1.z += heightOffset;
    p2.z += heightOffset;
    p3.z += heightOffset;
    q0.z += heightOffset;
    q1.z += heightOffset;
    q2.z += heightOffset;
    q3.z += heightOffset;

    mPoints.push_back(q0);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(0.0f, uv+dUV));
    mPoints.push_back(p0);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(0.0f, uv));

    mPoints.push_back(q1);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mUVs.push_back(Vector2(f1, uv+dUV));
    mPoints.push_back(p1);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mUVs.push_back(Vector2(f1, uv));

    mPoints.push_back(q2);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mUVs.push_back(Vector2(f2, uv+dUV));
    mPoints.push_back(p2);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mUVs.push_back(Vector2(f2, uv));

    mPoints.push_back(q3);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(1.0f, uv+dUV));
    mPoints.push_back(p3);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(1.0f, uv));

    // degenerate
    mPoints.push_back(p3);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(1.0f, uv));
    mPoints.push_back(q0);
    mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    mUVs.push_back(Vector2(0.0f, uv+dUV));
    
    uv += dUV;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Runway::MakeRow(
  const Vector3& start, const Transform& tm, const Vector3& offset, 
  float x, float dx, 
  float uv, float dUV, 
  float thisAlpha, float nextAlpha, 
  float frac, float heightOffset)
{
  float f1 = frac;
  float f2 = 1.0f - frac;
  // The points along
  Vector3 p0 = start + tm.RotateVec(Vector3(x, 0.0f, 0.0f));
  Vector3 p3 = p0 + offset;
  Vector3 p1 = p0 + f1 * (p3 - p0);
  Vector3 p2 = p0 + f2 * (p3 - p0);

  Vector3 q0 = start + tm.RotateVec(Vector3(x + dx, 0.0f, 0.0f));
  Vector3 q3 = q0 + offset;
  Vector3 q1 = q0 + f1 * (q3 - q0);
  Vector3 q2 = q0 + f2 * (q3 - q0);

  Environment::GetInstance().GetTerrain().GetTerrainHeight(p0, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(p1, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(p2, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(p3, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(q0, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(q1, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(q2, true);
  Environment::GetInstance().GetTerrain().GetTerrainHeight(q3, true);
  p0.z += heightOffset;
  p1.z += heightOffset;
  p2.z += heightOffset;
  p3.z += heightOffset;
  q0.z += heightOffset;
  q1.z += heightOffset;
  q2.z += heightOffset;
  q3.z += heightOffset;

  mPoints.push_back(q0);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(0.0f, uv+dUV));
  mPoints.push_back(p0);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(0.0f, uv));

  mPoints.push_back(q1);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, nextAlpha));
  mUVs.push_back(Vector2(f1, uv+dUV));
  mPoints.push_back(p1);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, thisAlpha));
  mUVs.push_back(Vector2(f1, uv));

  mPoints.push_back(q2);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, nextAlpha));
  mUVs.push_back(Vector2(f2, uv+dUV));
  mPoints.push_back(p2);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, thisAlpha));
  mUVs.push_back(Vector2(f2, uv));

  mPoints.push_back(q3);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(1.0f, uv+dUV));
  mPoints.push_back(p3);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(1.0f, uv));

  // degenerate
  mPoints.push_back(p3);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(1.0f, uv));
  mPoints.push_back(q0);
  mColours.push_back(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
  mUVs.push_back(Vector2(0.0f, uv+dUV));
}

//---------------------------------------------------------------------------------------------------------------------
Runway::~Runway()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderObject(this, RENDER_LEVEL_OBJECTS);
}

//---------------------------------------------------------------------------------------------------------------------
void Runway::RenderUpdate(class Viewport* viewport, int renderLevel)
{
  // Check if we need to set things up
  if (mMode == RUNWAY)
  {
    if (mPoints.empty())
      SetUpRunway();
  }
  else
  {
    const Aeroplane* playerAeroplane = PicaSim::GetInstance().GetPlayerAeroplane();
    if (!playerAeroplane || !playerAeroplane->GetPhysics())
      return;

    Vector3 pos = mTM.GetTrans();
    if (playerAeroplane->GetPhysics()->GetTetherHandlePos(pos))
    {
      pos.z = PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mRunwayPosition.z;
    }
    if (pos != mTM.GetTrans())
    {
      mTM.SetTrans(pos);
      SetUpCircle();
    }
  }

  // Render
  esPushMatrix();

  EnableLighting enableLighting;
  FrontFaceCW CW;
  EnableCullFace enableCullFace(GL_BACK);
  EnableBlend enableBlend;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  DisableDepthMask disableDepthMask;

  const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
  const TexturedModelShader* texturedModelShader = (TexturedModelShader*) ShaderManager::GetInstance().GetShader(SHADER_TEXTUREDMODEL);

  float specularAmount = 0.0f;
  float specularExponent = 100.0f;
  if (gGLVersion == 1)
  {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    GLfloat s[] = {specularAmount, specularAmount, specularAmount, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, specularExponent); // smooth surface = large numbers = small highlights

    // Overwrites the actual material for ambient and diffuse, front and back
    glEnable(GL_COLOR_MATERIAL);
  }
  else
  {
    texturedModelShader->Use();

    glUniform1i(texturedModelShader->u_texture, 0);
    glUniform1f(texturedModelShader->u_texBias, -0.5f);
    glUniform1f(texturedModelShader->u_specularExponent, specularExponent);
    glUniform1f(texturedModelShader->u_specularAmount, specularAmount);

    glEnableVertexAttribArray(texturedModelShader->a_position);
    glEnableVertexAttribArray(texturedModelShader->a_texCoord);
    glEnableVertexAttribArray(texturedModelShader->a_colour);

    glDisableVertexAttribArray(texturedModelShader->a_normal);

    esSetLighting(texturedModelShader->lightShaderInfo);
  }

  glActiveTexture(GL_TEXTURE0);

  if (gGLVersion == 1)
  {
    glVertexPointer(3, GL_FLOAT, 0, &mPoints[0].x);
    glNormal3f(0, 0, 1.0f);
    glTexCoordPointer(2, GL_FLOAT, 0, &mUVs[0].x);
    glColorPointer(4, GL_FLOAT, 0, &mColours[0].x);
  }
  else
  {
    glVertexAttribPointer(texturedModelShader->a_position, 3, GL_FLOAT, GL_FALSE, 0, &mPoints[0].x);
    glVertexAttribPointer(texturedModelShader->a_texCoord, 2, GL_FLOAT, GL_FALSE, 0, &mUVs[0].x);
    glVertexAttribPointer(texturedModelShader->a_colour, 4, GL_FLOAT, GL_FALSE, 0, &mColours[0].x);
    glVertexAttrib3f(texturedModelShader->a_normal, 0, 0, 1.0f);
  }

  esSetModelViewProjectionAndNormalMatrix(texturedModelShader->u_mvpMatrix, texturedModelShader->u_normalMatrix);

  if (mTexture.GetFlags() & Texture::UPLOADED_F)
  {
    glBindTexture(GL_TEXTURE_2D, mTexture.m_HWID);
  }

  glDrawArrays(GL_TRIANGLE_STRIP, 0, mPoints.size());

  if (gGLVersion == 1)
  {
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
  }

  if (mTexture.GetFlags() & Texture::UPLOADED_F)
  {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }

  esPopMatrix();
}

//---------------------------------------------------------------------------------------------------------------------
bool Runway::ContainsPoint(const Vector3& point) const
{
  Vector3 delta = point - mTM.GetTrans();
  delta.z = 0.0f;
  if (mMode == RUNWAY)
  {
    const Vector3 halfExtents = Vector3(mLength, mWidth, 0.0f) * 0.5f;
    float fwd = delta.Dot(mTM.RowX());
    if (fwd > halfExtents.x || fwd < -halfExtents.x)
      return false;
    float left = delta.Dot(mTM.RowY());
    if (left > halfExtents.y || left < -halfExtents.y)
      return false;
    return true;
  }
  else
  {
    float dist = delta.GetLength();
    if (dist > mLength - 0.5f * mWidth && dist < mLength + 0.5f * mWidth)
      return true;
    else
      return false;
  }
}
