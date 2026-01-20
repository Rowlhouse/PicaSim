#include "Challenge.h"
#include "GameSettings.h"
#include "Aeroplane.h"
#include "Environment.h"
#include "PicaSim.h"
#include "Menus/PicaDialog.h"

//======================================================================================================================
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

//======================================================================================================================
void Challenge::Validate()
{
    // Scoreloop has been removed - validation is no longer needed
    mValidated = false;
}

//======================================================================================================================
Challenge::ChallengeResult Challenge::UpdateChallenge(float deltaTime)
{
    GameSettings& gs = PicaSim::GetInstance().GetSettings();

    float flightTime = mAeroplane->GetFlightTime();
    if (flightTime > gs.mStatistics.mMaxFlightTime)
        gs.mStatistics.mMaxFlightTime = flightTime;
    gs.mStatistics.mTotalFlightTime += deltaTime;

    return CHALLENGE_CONTINUE;
}
