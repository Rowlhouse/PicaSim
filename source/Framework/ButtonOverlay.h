#ifndef BUTTON_OVERLAY_H
#define BUTTON_OVERLAY_H

#include "RenderOverlayObject.h"
#include "RenderManager.h"
#include <string>

#include "Graphics.h"

class ButtonOverlay : public RenderOverlayObject, public RenderGxObject
{
public:
  /// This indicates the point on the button to be used for positioning
  enum AnchorH
  {
    ANCHOR_H_LEFT = 0,
    ANCHOR_H_MID = 1,
    ANCHOR_H_RIGHT = 2
  };
  enum AnchorV
  {
    ANCHOR_V_TOP = 2,
    ANCHOR_V_MID = 1,
    ANCHOR_V_BOT = 0
  };

  /// Registers with render manager on creation
  ButtonOverlay(const char* imageFile, float size, float paddingFraction, AnchorH anchorH, AnchorV anchorV, 
    float x, float y, GLubyte alpha, bool enabled, bool enableText);
  ButtonOverlay(Texture* texture, float size, float paddingFraction, AnchorH anchorH, AnchorV anchorV, 
    float x, float y, GLubyte alpha, bool enabled, bool enableText);
  /// Deregisters on deletion
  ~ButtonOverlay();

  /// Returns true if the button has been pressed
  enum PressFlag {PRESS_DOWN = 1, PRESS_CLICKED = 2, PRESS_ANY = PRESS_DOWN | PRESS_CLICKED};
  bool IsPressed(uint32_t pressMask = PRESS_CLICKED) const;

  void RenderOverlayUpdate(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;
  void RenderOverlayUpdate(float deltaTime) OVERRIDE;
  void GxRender(int renderLevel, DisplayConfig& displayConfig) OVERRIDE;

  /// Enables/disables rendering and press checking
  void ForceDisable() {mEnabled = false; mAlpha = mAlphaRate = 0.0f;}
  void Enable(bool enable) {mEnabled = enable;}
  bool IsEnabled() const {return mEnabled;}

  void SetAlpha(GLubyte alpha) {mColour[3] = alpha;}
  void SetColour(GLubyte r, GLubyte g, GLubyte b) {mColour[0] = r; mColour[1] = g; mColour[2] = b;}
  void SetSize(float size) {mSize = size;}

  void SetAnchor(AnchorH anchorH, AnchorV anchorV) {mAnchorH = anchorH; mAnchorV = anchorV;}
  void SetPosition(float x, float y) {mX = x; mY = y;}

  // Sets the text, with anchors and position offsets. Offsets are in terms of the size. Anchor relates to the position on the text
  void SetText(const char* txt, class CIwGxFont* font, AnchorH textAnchorH, AnchorV textAnchorV, float offsetX, float offsetY);
  void UpdateText(const char* txt);
  void SetTextColour(const Vector4& colour);

private:
  void Init(float size, float paddingFraction, AnchorH anchorH, AnchorV anchorV, 
    float x, float y, GLubyte alpha, bool enabled, bool enableText);

  bool mEnabled;
  bool mEnableText;
  Texture* mTexture;
  bool     mOwnsTexture;
  float mSize; // As a fraction of the average width and height
  GLubyte mColour[4];

  std::string mText;
  CIwGxFont* mFont;
  AnchorH mTextAnchorH;
  AnchorV mTextAnchorV;
  float mTextOffsetX, mTextOffsetY;

  Vector4 mTextColour;
  AnchorH mAnchorH;
  AnchorV mAnchorV;
  float mPaddingFraction;
  float mX, mY;
  float mAlpha;
  float mAlphaRate;
  /// These get updated to store the bounding rectangle in fractions of the screen size
  float mX0, mY0, mX1, mY1;
};

#endif
