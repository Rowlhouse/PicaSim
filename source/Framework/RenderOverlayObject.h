#ifndef RENDEROVERLAY_H
#define RENDEROVERLAY_H

struct DisplayConfig;

/// Base class from which all objects that expect to be rendered as an overlay should derive
class RenderOverlayObject
{
public:
  /// Unregister from RenderManager before deleting
  virtual ~RenderOverlayObject() {};

  /// When this is called the projection will be set up to draw 
  /// with z coming out of the screen (clip range is -1 to 1), x going from 0 to width, y going 0 to height.
  virtual void RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig) = 0;

  /// Use to update the state
  virtual void RenderOverlayUpdate(float deltaTime) {}
};

#endif