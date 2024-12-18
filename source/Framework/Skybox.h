#ifndef SKYBOX_H
#define SKYBOX_H

#include "Helpers.h"
#include "Graphics.h"
#include "RenderObject.h"

#include <IwGx.h>

#include <vector>

class Skybox : public RenderObject
{
public:
  Skybox();
  ~Skybox();

  bool Init(const char* skyboxPath, bool use16BitTextures, int maxDetail, class LoadingScreenHelper* loadingScreen);
  void Terminate();

  void RenderUpdate(class Viewport* viewport, int renderLevel) OVERRIDE;

  void SetOffset(float degrees) {mOffset = degrees;}
  void SetExtendBelowHorizon(float amount) {mExtendBelowHorizon = amount;}

private:
  enum Side {UP, FRONT, LEFT, BACK, RIGHT, DOWN, NUM_SIDES};

  void DrawSide(Side side, int mvpLoc) const;

  typedef std::vector<Texture*> Textures;
  Textures mTextures[NUM_SIDES];
  float mOffset;
  float mExtendBelowHorizon;
  bool mInitialised;
};

#endif