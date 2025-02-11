#include "ChallengeFreeFly.h"
#include "AIControllerGlider.h"
#include "AIControllerPowered.h"
#include "AeroplanePhysics.h"
// #include "ClassesFontMarmelade.h"
#include "FonctionsMarmelade.h"
#include "Menu.h"
#include "Trace.h"
#include "PicaSim.h"
#include "Environment.h"
// #include "Aeroplane.h"
// #include "AeroplanePhysics.h"
// #include "AIControllerGlider.h"
// #include "AIControllerPowered.h"
// #include "Menus/Menu.h"

// #include <IwGx.h>
// #include <IwGxFont.h>

//----------------------------------------------------------------------------------------------------------------------
void ChallengeFreeFly::Relaunched()
{
  mMaxSpeed = 0.0f;
  mOnGroundTime = 0.0f;
  mSmoothedAscentRate = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeFreeFly::Init(Aeroplane* aeroplane, LoadingScreenHelper* loadingScreen)
{
  TRACE_METHOD_ONLY(1);
  mAeroplane = aeroplane;
  RenderManager::GetInstance().RegisterRenderGxObject(this, 0);
  mNeedToCacheText = true;

  mTextBackgroundMaterial = new CIwMaterial;
  mTextBackgroundMaterial->SetAlphaMode(CIwMaterial::ALPHA_BLEND);

  Relaunched();

  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const AIControllersSettings& aics = gs.mAIControllersSettings;

  int numAIControllers = 0;

  while (numAIControllers < gs.mOptions.mFreeFlightMaxAI)
  {
    bool gotOne = false;
    for (size_t i = 0 ; i != aics.mAIControllers.size() && numAIControllers < gs.mOptions.mFreeFlightMaxAI; ++i)
    {
      const AIControllersSettings::AIControllerSetting& aic = aics.mAIControllers[i];

      bool available = true;
      AeroplaneSettings as;
      if (!as.LoadFromFile(aic.mAeroplaneFile.c_str()))
      {
        available = false;
      }
      else
      {
        if (as.mAvailability > gs.mStatistics.mAppType)
          available = false;
      }

      AIController* controller = 0;
      if (available)
      {
        switch (as.mAIType)
        {
        case AeroplaneSettings::AITYPE_GLIDER:
          controller = new AIControllerGlider(aic, numAIControllers);
          break;
        case AeroplaneSettings::AITYPE_POWERED:
          controller = new AIControllerPowered(aic, numAIControllers);
          break;
        default:
          break;
        }
      }
      if (controller)
      {
        if (controller->Init(loadingScreen))
        {
          mAIControllers.push_back(controller);

          ++numAIControllers;
          gotOne = true;
        }
        else
        {
          delete controller;
        }
      }
    }

    if (!gotOne)
      break;

    if (!aics.mCreateMaxNumControllers)
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeFreeFly::Reset()
{
  for (size_t i = 0 ; i != mAIControllers.size() ; ++i)
  {
    AIController* aic = mAIControllers[i];
    aic->Reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeFreeFly::Terminate()
{
  TRACE_METHOD_ONLY(1);
  RenderManager::GetInstance().UnregisterRenderGxObject(this, 0);

  delete mTextBackgroundMaterial;

  while (!mAIControllers.empty())
  {
    AIController* controller = mAIControllers.back();
    controller->Terminate();
    delete controller;
    mAIControllers.pop_back();
  }
}

//----------------------------------------------------------------------------------------------------------------------
Challenge::ChallengeResult ChallengeFreeFly::UpdateChallenge(float deltaTime)
{
  Challenge::UpdateChallenge(deltaTime);

  GameSettings& gs = PicaSim::GetInstance().GetSettings();

  // TODO better metric for this
  float speed = mAeroplane->GetVelocity().GetLength();
  bool touching = mAeroplane->GetPhysics()->GetContactTime() > 0.0f;

  if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_AEROTOW)
    touching = false;

  if (speed > mMaxSpeed)
    mMaxSpeed = speed;

  if (speed < 1.0f && touching)
    mOnGroundTime += deltaTime;
  else
    mOnGroundTime = 0.0f;

  if (mOnGroundTime > gs.mAeroplaneSettings.mRelaunchTime && gs.mAeroplaneSettings.mRelaunchWhenStationary)
  {
    mOnGroundTime = 0.0f;
    return CHALLENGE_RELAUNCH;
  }

  if (mAeroplane->GetCrashed() && mOnGroundTime > gs.mAeroplaneSettings.mRelaunchTime)
  {
    mOnGroundTime = 0.0f;
    return CHALLENGE_RELAUNCH;
  }

  return CHALLENGE_CONTINUE;
}

//----------------------------------------------------------------------------------------------------------------------
const char* GetSpeedUnitText(const Options& options)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  default:
    return "m/s";
  case Options::UNITS_KPH:
    return "km/h";
  case Options::UNITS_MPH:
    return "mph";
  }
}

//----------------------------------------------------------------------------------------------------------------------
const float GetSpeed(const Options& options, float speed)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  default:
    return speed;
  case Options::UNITS_KPH:
    return speed * 3.6f;
  case Options::UNITS_MPH:
    return speed * 2.23693629f; // to mph
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* GetAscentRateUnitText(const Options& options)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  case Options::UNITS_KPH:
  default:
    return "m/s";
  case Options::UNITS_MPH:
    return "ft/s";
  }
}

//----------------------------------------------------------------------------------------------------------------------
const float GetAscentRate(const Options& options, float speed)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  case Options::UNITS_KPH:
  default:
    return speed;
  case Options::UNITS_MPH:
    return speed * 3.2808399f; // to ft/s
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* GetDistanceUnitText(const Options& options)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  case Options::UNITS_KPH:
  default:
    return "m";
  case Options::UNITS_MPH:
    return "ft";
  }
}

//----------------------------------------------------------------------------------------------------------------------
const float GetDistance(const Options& options, float distance)
{
  switch (options.mFreeFlightUnits)
  {
  case Options::UNITS_MPS:
  case Options::UNITS_KPH:
  default:
    return distance;
  case Options::UNITS_MPH:
    return distance * 3.2808399f; // to feet
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChallengeFreeFly::GxRender(int renderLevel, DisplayConfig& displayConfig)
{
  TRACE_METHOD_ONLY(2);
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();
  const Options& options = gs.mOptions;
  if (
    !options.mFreeFlightDisplayAltitude && 
    !options.mFreeFlightDisplayDistance && 
    !options.mFreeFlightDisplayAscentRate && 
    !options.mFreeFlightDisplaySpeed && 
    !options.mFreeFlightDisplayAirSpeed && 
    !options.mFreeFlightDisplayMaxSpeed && 
    !options.mFreeFlightDisplayTime &&
    !mAeroplane->GetCrashed())
  {
    return;
  }

  if (!PicaSim::GetInstance().GetShowUI())
    return;

  CIwGxFont* font = GetFont(gs);
  IwGxFontSetFont(font);

  Colour origColour = IwGxFontGetCol();

  IwGxLightingOn();
  uint16_t fontHeight = font->GetHeight();
  CIwRect textRect;
  if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_FLYING && options.mFreeFlightTextAtTop)
    textRect = CIwRect(displayConfig.mLeft, displayConfig.mBottom + fontHeight*1/4,(int16_t)displayConfig.mWidth,fontHeight);
  else
    textRect = CIwRect(displayConfig.mLeft,(int16_t)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16_t)displayConfig.mWidth,fontHeight);
  IwGxFontSetRect(textRect);
  IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_CENTRE);

  IwChar txt[256] = "";
  IwChar tmp[64] = "";

  const CameraTarget* cameraTarget = PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTarget();
  const Aeroplane* aeroplane = dynamic_cast<const Aeroplane*>(cameraTarget);
  if (!aeroplane)
    aeroplane = dynamic_cast<const Aeroplane*>(PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTransform());
  if (!aeroplane)
    aeroplane = mAeroplane;

  int crashFlags = aeroplane->GetCrashFlags();
  if (crashFlags)
  {
    Language language = options.mLanguage;
    sprintf(txt, "%s:", TXT(PS_CRASHED));
    if (crashFlags & Aeroplane::CRASHFLAG_AIRFRAME)
      sprintf(txt + strlen(txt), " %s", TXT(PS_AIRFRAME));
    if (crashFlags & Aeroplane::CRASHFLAG_PROPELLER)
      sprintf(txt + strlen(txt), " %s", TXT(PS_PROPELLER));
    if (crashFlags & Aeroplane::CRASHFLAG_UNDERCARRIAGE)
      sprintf(txt + strlen(txt), " %s", TXT(PS_UNDERCARRIAGE));
    IwGxFontSetCol(0xff0000ff);
  }
  else
  {
    if (options.mFreeFlightDisplayTime)
    {
      float flightTime = aeroplane->GetFlightTime();
      int minutes = (int) (flightTime / 60.0f);
      float seconds = flightTime - minutes * 60.0f;
      if (minutes > 0)
        sprintf(tmp, "%dm%04.1fs", minutes, seconds);
      else
        sprintf(tmp, "%4.1fs", seconds);
      strcat(txt, tmp);
    }

    if (options.mFreeFlightDisplaySpeed)
    {
      float speed = aeroplane->GetVelocity().GetLength();
      sprintf(tmp, "  %5.1f%s", GetSpeed(options, speed), GetSpeedUnitText(options));
      strcat(txt, tmp);
    }

    if (options.mFreeFlightDisplayAirSpeed)
    {
      Vector3 windVel = Environment::GetInstance().GetWindAtPosition(aeroplane->GetTransform().GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);
      float speed = (aeroplane->GetVelocity() - windVel).GetLength();
      sprintf(tmp, "  %5.1f%s", GetSpeed(options, speed), GetSpeedUnitText(options));
      strcat(txt, tmp);
    }

    if (options.mFreeFlightDisplayMaxSpeed)
    {
      sprintf(tmp, "  (max %5.1f%s)", GetSpeed(options, mMaxSpeed), GetSpeedUnitText(options));
      strcat(txt, tmp);
    }

    float ascentRate = aeroplane->GetVelocity().z;
    mSmoothedAscentRate += (ascentRate - mSmoothedAscentRate) * 0.01f;
    if (options.mFreeFlightDisplayAscentRate)
    {
#ifdef IW_DEBUG
      sprintf(tmp, "  %5.2f%s (%5.2f%s)", GetAscentRate(options, ascentRate), GetAscentRateUnitText(options), GetAscentRate(options, mSmoothedAscentRate), GetAscentRateUnitText(options));
#else
      sprintf(tmp, "  %5.2f%s", GetAscentRate(options, ascentRate), GetAscentRateUnitText(options));
#endif
      strcat(txt, tmp);
    }

    if (options.mFreeFlightDisplayAltitude)
    {
      float z = aeroplane->GetTransform().GetTrans().z;
      float launchZ = PicaSim::GetInstance().GetObserver().GetTransform().GetTrans().z;
      sprintf(tmp, "  %5.1f%s", GetDistance(options, z - launchZ), GetDistanceUnitText(options));
      strcat(txt, tmp);
    }

    if (options.mFreeFlightDisplayDistance)
    {
      Vector3 delta = aeroplane->GetTransform().GetTrans() - PicaSim::GetInstance().GetObserver().GetTransform().GetTrans();
      float dist = delta.GetLength();
      sprintf(tmp, "  %5.1f%s", GetDistance(options, dist), GetDistanceUnitText(options));
      strcat(txt, tmp);
    }

    if (options.mFreeFlightColourText)
    {
      // 1 is up, 0 is down
      float ascentAmount = ClampToRange(0.5f + ascentRate / 12.0f, 0.0f, 1.0f);
      int r = ClampToRange((int) ((1.0f - ascentAmount)*255.0f), 0, 255);
      int g = ClampToRange((int) (ascentAmount * 255.0f), 0, 255);
      int b = 0;  
      int32_t col = r + (g << 8) + (b << 16) + (255 << 24);
      IwGxFontSetCol(col);
    }
    else
    {
      IwGxFontSetCol(0xff000000);
    }
  }

  // Text background
  if (options.mFreeFlightTextBackgroundOpacity > 0.0f)
  {
    uint32_t w = displayConfig.mWidth;
    uint32_t x = strlen(txt) * fontHeight * 1 / 2;
    uint32_t x0 = (w - x) / 2;
    mTextBackgroundXY = CIwSVec2(x0, textRect.y);
    mTextBackgroundWH = CIwSVec2(x, textRect.h);
    int c = ClampToRange((int) (options.mFreeFlightTextBackgroundColour * 255), 0, 255);
    int a = ClampToRange((int) (options.mFreeFlightTextBackgroundOpacity * 255), 0, 255);
    mTextBackgroundMaterial->SetColAmbient(c, c, c, a);
    IwGxSetMaterial(mTextBackgroundMaterial);
    IwGxDrawRectScreenSpace(&mTextBackgroundXY, &mTextBackgroundWH);
  }
  IwGxFontDrawText(txt);

  if (gs.mOptions.mDisplayFPS)
  {
    IwGxFontSetRect(CIwRect(displayConfig.mLeft,(int16_t)(displayConfig.mBottom + displayConfig.mHeight - fontHeight*5/4),(int16_t)displayConfig.mWidth,fontHeight));
    IwGxFontSetAlignmentHor(IW_GX_FONT_ALIGN_RIGHT);
    sprintf(txt, "%d", (int) (gs.mStatistics.mSmoothedFPS + 0.5f));
    IwGxFontSetCol(0xff00ffff);
    IwGxFontDrawText(txt);
  }

  if (mNeedToCacheText)
  {
    IwGxFontSetCol(0x00ffffff);
    IwGxFontDrawText("0123456789.");
    mNeedToCacheText = false;
  }

  IwGxFontSetCol(origColour);
  IwGxLightingOff();
}

