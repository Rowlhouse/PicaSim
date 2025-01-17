#include "Challenge.h"
#include "PicaDialog.h"
#include "Trace.h"
// #include "GameSettings.h"
// #include "Aeroplane.h"
#include "Environment.h"
#include "PicaSim.h"
// #include "Menus/PicaDialog.h"

//----------------------------------------------------------------------------------------------------------------------
Challenge::Challenge(GameSettings& gameSettings)
{
  TRACE_METHOD_ONLY(1);

  gameSettings.mLightingSettings.LoadFromFile(gameSettings.mChallengeSettings.mLightingSettingsFile, true);
  gameSettings.mEnvironmentSettings.LoadFromFile(gameSettings.mChallengeSettings.mEnvironmentSettingsFile, true);
  bool gotAeroplane = 
    gameSettings.mChallengeSettings.mAeroplaneSettingsFile.length() > 1 && 
    gameSettings.mAeroplaneSettings.LoadFromFile(gameSettings.mChallengeSettings.mAeroplaneSettingsFile, true);

  if (gameSettings.mChallengeSettings.mWindSpeedOverride >= 0.0f)
    gameSettings.mEnvironmentSettings.mWindSpeed = gameSettings.mChallengeSettings.mWindSpeedOverride;

  if (gameSettings.mChallengeSettings.mTurbulenceOverride >= 0.0f)
    gameSettings.mEnvironmentSettings.mTurbulenceAmount = gameSettings.mChallengeSettings.mTurbulenceOverride;

  if (gameSettings.mChallengeSettings.mThermalDensityOverride >= 0.0f)
    gameSettings.mEnvironmentSettings.mThermalDensity = gameSettings.mChallengeSettings.mThermalDensityOverride;

  if (
    gotAeroplane &&
    gameSettings.mOptions.mUseAeroplanePreferredController && 
    !gameSettings.mAeroplaneSettings.mPreferredController.empty()
    )
  {
    TRACE_FILE_IF(1) TRACE("Loading Controller %s", gameSettings.mAeroplaneSettings.mPreferredController.c_str());
    bool controllerResult = gameSettings.mControllerSettings.LoadFromFile(gameSettings.mAeroplaneSettings.mPreferredController, true);
    TRACE_FILE_IF(1) TRACE(" %s\n", controllerResult ? "success" : "failed");
  }

  bool objectsResult = gameSettings.mObjectsSettings.LoadFromFile(gameSettings.mEnvironmentSettings.mObjectsSettingsFile, true);
  IwAssert(ROWLHOUSE, objectsResult);
}

//----------------------------------------------------------------------------------------------------------------------
void Challenge::Validate()
{
  const GameSettings& gs = PicaSim::GetInstance().GetSettings();

  if (!gs.mOptions.mEnableScoreloop)
  {
    mValidated = false;
    return;
  }

  uint32_t checksum = gs.mChallengeSettings.mChecksum;
  if (!gs.mChallengeSettings.mAllowAeroplaneSettings)
    checksum += mAeroplane->GetChecksum();
  checksum += Environment::GetInstance().GetChecksum();

  if (gs.mChallengeSettings.mChallengeID == 85 && checksum == 4251955992u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 86 && checksum == 517577937u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 87 && checksum == 386069371u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 88 && checksum == 2735722138u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 89 && checksum == 203932621u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 90 && checksum == 3478348682u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 91 && checksum == 1367594518u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 92 && checksum == 747317556u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 93 && checksum == 2984828080u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 94 && checksum == 49282245u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 95 && checksum == 3378330593u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 96 && checksum == 2855825047u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 97 && checksum == 1683139988u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 98 && checksum == 3224991258u)
    mValidated = true;
  else if (gs.mChallengeSettings.mChallengeID == 99 && checksum == 3973735332u)
    mValidated = true;
  else
    mValidated = false;

  if (!mValidated)
  {
    const Language language = gs.mOptions.mLanguage;
    ShowDialog("PicaSim", "This challenge is not validated so the results will not be uploaded to Scoreloop", TXT(PS_OK));
  }
}

//----------------------------------------------------------------------------------------------------------------------
Challenge::ChallengeResult Challenge::UpdateChallenge(float deltaTime)
{
  GameSettings& gs = PicaSim::GetInstance().GetSettings();

  float flightTime = mAeroplane->GetFlightTime();
  if (flightTime > gs.mStatistics.mMaxFlightTime)
    gs.mStatistics.mMaxFlightTime = flightTime;
  gs.mStatistics.mTotalFlightTime += deltaTime;

  return CHALLENGE_CONTINUE;
}
