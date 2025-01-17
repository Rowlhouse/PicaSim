#include "TrialOverlay.h"
#include "Menu.h"
#include "Viewport.h"

//#include <s3e.h>
//#include <IwGx.h>
//#include <IwGxFont.h>

//---------------------------------------------------------------------------------------------------------------------
TrialOverlay::TrialOverlay(const GameSettings& gameSettings, float timeRemaining)
{
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);
  mFont = GetFont(gameSettings);
}

//---------------------------------------------------------------------------------------------------------------------
TrialOverlay::~TrialOverlay()
{
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);
}

//---------------------------------------------------------------------------------------------------------------------
void TrialOverlay::GxRender(int renderLevel, DisplayConfig& displayConfig) 
{
  if (mTimeRemaining < 0.0f)
    return;

  IwGxFontSetFont(mFont);

  Colour origColour = IwGxFontGetCol();
  CIwColour colour;
  colour.Set(255, 255, 255);
  IwGxFontSetCol(colour);
  IwGxLightingOn();
  uint16_t fontHeight = mFont->GetHeight();

  float yf = displayConfig.mBottom + 0.2f * displayConfig.mHeight;
  int16_t y = (int16_t) yf;

  IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);
  IwGxFontSetAlignmentVer(IW_GX_FONT_ALIGN_MIDDLE);

  IwGxFontSetRect(CIwRect(0, y, displayConfig.mWidth, fontHeight));
  char txt[256];
  sprintf(txt, "Trial time remaining: %4.0f seconds", mTimeRemaining);
  IwGxFontDrawText(txt);

  IwGxFontSetCol(origColour);
  IwGxLightingOff();
}
