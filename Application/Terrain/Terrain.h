#ifndef TERRAIN_H
#define TERRAIN_H

//#include <s3eTypes.h>
//#include <IwGx.h>

//#include <vector>

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "ClassesMarmelade.h"
#include "FrameBufferObject.h"
#include "HeightfieldRuntime.h"
#include "RenderObject.h"
#include "Vecteurs.h"
#include "Helpers.h"

float interpolateForZ(
  Vector3 pos,
  const Vector3& pos11,
  const Vector3& pos12,
  const Vector3& pos21,
  const Vector3& pos22,
  int downwardSlope); // direction of the diagonal

class Terrain : public RenderObject
{
public:
  Terrain();
  ~Terrain();

  void Init(btDynamicsWorld& dynamicsWorld, class LoadingScreenHelper* loadingScreen, uint32_t& checksum);
  void Terminate();

  void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

  /// Returns the nearest point on the terrrain surface to pos (only accurate when it is close), and
  /// the local normal vector. If clampToPlain is set then the returned height is no lower than the
  /// plain height.
  void GetLocalTerrain(
    const Vector3 & pos,
    Vector3 & terrain_pos,
    Vector3 & normal,
    bool clampToPlain) const;

  /// Returns terrain height
  float GetTerrainHeight(float x1, float y1, bool clampToPlain) const;
  // Approximate & fast version
  float GetTerrainHeightFast(float x1, float y1, bool clampToPlain) const;
  /// Sets the height in the position object
  void GetTerrainHeight(Vector3 & pos, bool clampToPlain) const;
  // Approximate & fast version
  void GetTerrainHeightFast(Vector3 & pos, bool clampToPlain) const;
  /// Gets the terrain extents
  void GetTerrainRange(float& xMin, float& xMax, float& yMin, float& yMax) const;

  Vector3 GetTerrainMidpoint(bool clampToPlain) const;

  void ClearLOD() {mHeightfield->clearSaved();}

  const Heightfield::HeightfieldRuntime& GetHeightfield() const {return *mHeightfield;}

  /// Returns (by reference) the index for the quad surrounding x0, y0.
  /// The other indexes are i11+1 etc
  void GetIndex(
    float x0, float y0,
    unsigned int & i11, unsigned int & j11) const;

private:
  void RenderShadow(RenderObject& shadowCaster, Viewport* viewport);
  void RenderShadow(RenderObject& shadowCaster, const Vector3& shadowCasterPos, const float shadowCasterRadius, Viewport* viewport);

  void RenderPlain(class Viewport* viewport);
  void RenderHeightfield(class Viewport* viewport);

  void CalculateLightmapTexture(unsigned int size, float gamma, const char* basicTexture, Vector3& plainColour, class LoadingScreenHelper* loadingScreen);

  void InitPhysics(class LoadingScreenHelper* loadingScreen);

  Heightfield::HeightfieldRuntime* mHeightfield;
  Heightfield::Vertex* mVertexOut;
  float mHeightfieldBoxSize;

  /// Bullet needs us to maintain the heightfield data
  std::vector<float> mPhysicsHeightfield;

  Texture mHeightfieldTexture;
  Texture mDetailTexture;

  static const int mNumPlainPts = 11;
  GLfloat mPlainPts[mNumPlainPts*3];
  GLfloat mPlainCols[mNumPlainPts*4];

  static const int mNumOuterPlainPts = mNumPlainPts - 1;
  GLfloat mOuterPlainPts[mNumOuterPlainPts*3*2];
  GLfloat mOuterPlainCols[mNumOuterPlainPts*4*2];

  btDynamicsWorld* mDynamicsWorld;

  btCollisionShape* mPlainCollisionShape;
  btRigidBody*      mPlainRigidBody;

  btHeightfieldTerrainShape* mHeightfieldTerrainShape;
  btRigidBody*      mHeightfieldRigidBody;

  Texture* mGenericShadowTexture;
  FrameBufferObject* mShadowFrameBufferObject;

  // To check if we need to regen LOD
  float mLastLOD;
  Vector3 mLastCameraPosition;

  GLuint mTerrainVertexBuffer;
};

#endif
