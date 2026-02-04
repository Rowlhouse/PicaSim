#include "AIControllerGlider.h"
#include "Menus/PicaDialog.h"
#include "AeroplanePhysics.h"
#include "AeroplaneGraphics.h"
#include "Aeroplane.h"
#include "AIControllerTug.h"
#include "PicaSim.h"

//======================================================================================================================
Vector3 AIControllerGlider::GetLaunchPos() const
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    Vector3 observerCameraPosition = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();
    float altitude = observerCameraPosition.z - Environment::GetInstance().GetTerrain().GetTerrainHeight(observerCameraPosition.x, observerCameraPosition.y, true);

    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(observerCameraPosition, Environment::WIND_TYPE_SMOOTH);
    Vector3 horWindDir(windVel.x, windVel.y, 0.0f);
    SafeNormalise(horWindDir, Vector3(1, 0, 0));
    const Vector3 horWindLeftDir = horWindDir.Cross(Vector3(0.0f, 0.0f, 1.0f));

    Quat q(horWindDir, -(1.0f + aics.mLaunchDirection) * PI * 0.5f);
    const Vector3 launchDir = q.RotateVector(horWindLeftDir);

    Vector3 launchPos = observerCameraPosition + launchDir *aics.mLaunchSeparationDistance * float(mAIControllerIndex+2);

    launchPos.z = Maximum(launchPos.z, altitude + Environment::GetInstance().GetTerrain().GetTerrainHeight(launchPos.x, launchPos.y, true));

    return launchPos;
}

//======================================================================================================================
AIControllerGlider::AIControllerGlider(const AIControllersSettings::AIControllerSetting& aiControllerSetting, int AIControllerIndex)
    : mAIControllerSetting(aiControllerSetting)
{
    mAeroplane = 0;
    mAIControllerIndex = AIControllerIndex;
}

//======================================================================================================================
bool AIControllerGlider::Init(LoadingScreenHelper* loadingScreen)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();

    for (unsigned int i = 0 ; i != MAX_CHANNELS ; ++i)
        mOutputControls[i] = 0.0f;

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_CONTROL);

    AeroplaneSettings as;
    if (!as.LoadFromFile(mAIControllerSetting.mAeroplaneFile.c_str()))
    {
        const Language language = gs.mOptions.mLanguage;
        ShowDialog("PicaSim", "Failed to load AI controller", TXT(PS_OK));
        return false;
    }

    // Override a few aeroplane settings
    as.mColourOffset += mAIControllerSetting.mColourOffset;

    float r1 = rand() / float(RAND_MAX);
    as.mColourOffset += (r1 - 0.5f) * gs.mAIControllersSettings.mRandomColourOffset;
    as.mColourOffset = WrapToRange(as.mColourOffset, 0.0f, 1.0f);

    mAeroplane = std::make_unique<Aeroplane>(*this);
    Vector3 launchPos = GetLaunchPos();
    mAeroplane->Init(as, &launchPos, loadingScreen);

    Reset();

    for(size_t i = 0 ; i != MAX_CHANNELS ; ++i)
    {
        mOutputControls[i] = 0.0f;
    }
    return true;
}

//======================================================================================================================
void AIControllerGlider::Reset()
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIAeroplaneSettings& aias = mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings;
    const AIEnvironmentSettings& aies = gs.mEnvironmentSettings.mAIEnvironmentSettings;
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    Vector3 launchPos = GetLaunchPos();
    mAeroplane->Launch(launchPos);

    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(launchPos, Environment::WIND_TYPE_GUSTY);
    ChooseNewWaypoint(aies, aias, windVel, launchPos, true);
}


//======================================================================================================================
void AIControllerGlider::Relaunched()
{
    Vector3 launchPos = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();
    launchPos += Vector3(0,0,2) * float(mAIControllerIndex+1);

    mAeroplane->Launch(launchPos);

    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIAeroplaneSettings& aias = mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings;
    const AIEnvironmentSettings& aies = gs.mEnvironmentSettings.mAIEnvironmentSettings;
    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(launchPos, Environment::WIND_TYPE_GUSTY);
    ChooseNewWaypoint(aies, aias, windVel, launchPos, true);
}


//======================================================================================================================
void AIControllerGlider::Terminate()
{
    TRACE_METHOD_ONLY(1);
    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_CONTROL);
    PicaSim::GetInstance().RemoveCameraTarget(mAeroplane.get());

    if (mAeroplane)
    {
        mAeroplane->Terminate();
        mAeroplane.reset();
    }
}

//======================================================================================================================
void AIControllerGlider::ChooseNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    const Vector3& windVel,
    const Vector3& pos,
    bool isLaunch)
{
    const Vector3& observerPos = PicaSim::GetInstance().GetObserver().GetCameraTransform(0).GetTrans();
    Vector3 horWindDir(windVel.x, windVel.y, 0.0f);
    SafeNormalise(horWindDir, Vector3(1,0,0));
    Vector3 horWindSideDir = horWindDir.Cross(Vector3(0,0,1));

    if (aies.mSceneType == AIEnvironmentSettings::SCENETYPE_SLOPE)
    {
        mWaypointTimer = aias.mSlopeMaxWaypointTime;

        for (size_t iTry = 0 ; iTry != 10 ; ++iTry)
        {
            float r1 = rand() / float(RAND_MAX);
            float r2 = rand() / float(RAND_MAX);
            float r3 = rand() / float(RAND_MAX);

            mTargetWaypoint = observerPos;
            mTargetWaypoint   -= (aias.mSlopeMinUpwindDistance + r1 * (aias.mSlopeMaxUpwindDistance - aias.mSlopeMinUpwindDistance)) * horWindDir;
            mTargetWaypoint   += (aias.mSlopeMinLeftDistance   + r2 * (aias.mSlopeMaxLeftDistance   - aias.mSlopeMinLeftDistance))     * horWindSideDir;
            mTargetWaypoint.z += (aias.mSlopeMinUpDistance     + r3 * (aias.mSlopeMaxUpDistance     - aias.mSlopeMinUpDistance));

            const float terrainHeight = Environment::GetInstance().GetTerrain().GetTerrainHeightFast(mTargetWaypoint.x, mTargetWaypoint.y, true);

            if (mTargetWaypoint.z - terrainHeight > aias.mGliderControlMinAltitude)
                return;
            mTargetWaypoint.z = Maximum(mTargetWaypoint.z, terrainHeight + aias.mGliderControlMinAltitude);
        }
    }
    else
    {
        if (isLaunch && mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_NONE)
        {
            // Hand launch
            mTargetWaypoint = observerPos;
            float dist = aias.mFlatMaxDistance * 10.0f;
            Vector3 delta = mAeroplane->GetTransform().RowX() * dist;
            delta.z = dist;
            mTargetWaypoint += delta;
            mWaypointTimer = 4.0f;
            return;
        }

        mWaypointTimer = aias.mFlatMaxWaypointTime;

        float thermalDist = FLT_MAX;
        Vector3 thermalPos = Environment::GetInstance().GetThermalManager().GetNearestThermalPosition(pos, thermalDist);
        Vector3 horDeltaFromObserver = thermalPos - observerPos;
        horDeltaFromObserver.z = 0.0f;

        if (horDeltaFromObserver.GetLength() < aias.mFlatMaxDistance * 2.0f && thermalDist < FLT_MAX)
        {
            mTargetWaypoint = thermalPos;
            mTargetWaypoint.z = pos.z + aias.mFlatMaxDistance;
            return;
        }

        for (size_t iTry = 0 ; iTry != 10 ; ++iTry)
        {
            float r1 = rand() / float(RAND_MAX);
            float r2 = rand() / float(RAND_MAX);

            mTargetWaypoint = observerPos;
            mTargetWaypoint   += (aias.mFlatMaxDistance * (2.0f * r1 - 1.0f)) * horWindDir;
            mTargetWaypoint   += (aias.mFlatMaxDistance * (2.0f * r2 - 1.0f)) * horWindSideDir;
            mTargetWaypoint.z += aias.mFlatMaxDistance;

            const float terrainHeight = Environment::GetInstance().GetTerrain().GetTerrainHeightFast(mTargetWaypoint.x, mTargetWaypoint.y, true);

            if (mTargetWaypoint.z - terrainHeight > aias.mGliderControlMinAltitude)
                return;
            mTargetWaypoint.z = Maximum(mTargetWaypoint.z, terrainHeight + aias.mGliderControlMinAltitude);
        }
    }
}

//======================================================================================================================
void AIControllerGlider::CheckForNewWaypoint(
    const AIEnvironmentSettings& aies,
    const AIAeroplaneSettings& aias,
    const Vector3& pos,
    const Vector3& windVel,
    Vector3& deltaToTarget, 
    Vector3& horDeltaToTarget)
{
    // Do different checks depending on if we are aerotowing
    const float altitude = fabsf(pos.z -  Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true));
    const float physicsRadius = mAeroplane->GetPhysics()->getAABBRadius();
    const float contactTime = mAeroplane->GetPhysics()->GetContactTime();
    const float contactTimeForRelaunch = 10.0f;

    if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_AEROTOW && mAeroplane->GetTugController())
    {
        if (mAeroplane->GetCrashed())
        {
            Vector3 launchPos = GetLaunchPos();
            mAeroplane->Launch(launchPos);
            ChooseNewWaypoint(aies, aias, windVel, pos, true);
            return;
        }

        // Relaunch if on the ground and the tug is connected and crashed
        if (contactTime > contactTimeForRelaunch && altitude < physicsRadius && mAeroplane->GetTugController()->GetAeroplane()->GetCrashed())
        {
            Vector3 launchPos = GetLaunchPos();
            mAeroplane->Launch(launchPos);
            ChooseNewWaypoint(aies, aias, windVel, pos, true);
            return;
        }

        if (altitude > mAeroplane->GetAeroplaneSettings().mAeroTowHeight * 0.95f)
        {
            mAeroplane->SetLaunchMode(Aeroplane::LAUNCHMODE_NONE);
            ChooseNewWaypoint(aies, aias, windVel, pos, false);
        }
        const Aeroplane* tugAeroplane = mAeroplane->GetTugController()->GetAeroplane();
        mWaypointTimer = 1.0f;
        mTargetWaypoint = tugAeroplane->GetTransform().GetTrans();
    }
    else
    {
        if (contactTime > contactTimeForRelaunch && altitude < physicsRadius)
        {
            Vector3 launchPos = GetLaunchPos();
            mAeroplane->Launch(launchPos);
            ChooseNewWaypoint(aies, aias, windVel, pos, true);
        }
        else
        {
            if (mAeroplane->GetLaunchMode() == Aeroplane::LAUNCHMODE_BUNGEE)
            {
                mTargetWaypoint = 
                    pos - GetSafeNormalised(windVel) * aias.mWaypointTolerance * 2.0f + 
                    Vector3(0, 0, aias.mWaypointTolerance * 2.0f);
            }
            else
            {
                float horDistToTarget = horDeltaToTarget.GetLength();
                if (
                    horDistToTarget < aias.mWaypointTolerance ||
                    mWaypointTimer < 0.0f)
                {
                    ChooseNewWaypoint(aies, aias, windVel, pos, false);
                    deltaToTarget = mTargetWaypoint - pos;
                    horDeltaToTarget = Vector3(deltaToTarget.x,deltaToTarget.y,0.0f);
                    horDistToTarget = horDeltaToTarget.GetLength();
                }
            }
        }
    }
}

//======================================================================================================================
void AIControllerGlider::EntityUpdate(float deltaTime, int entityLevel)
{
    const GameSettings& gs = PicaSim::GetInstance().GetSettings();
    const AIAeroplaneSettings& aias = mAeroplane->GetAeroplaneSettings().mAIAeroplaneSettings;
    const AIEnvironmentSettings& aies = gs.mEnvironmentSettings.mAIEnvironmentSettings;
    const AIControllersSettings& aics = gs.mAIControllersSettings;

    const Transform tm = mAeroplane->GetTransform();
    const Vector3 pos = mAeroplane->GetTransform().GetTrans();
    const Vector3 windVel = Environment::GetInstance().GetWindAtPosition(pos, Environment::WIND_TYPE_GUSTY);
    const Vector3 vel = mAeroplane->GetVelocity();
    const Vector3 velRelWind = vel - windVel;
    const float physicsRadius = mAeroplane->GetPhysics()->getAABBRadius();
    const float altitude = fabsf(pos.z -  Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true));

    // Update the camera target status if necessary
    PicaSim::GetInstance().AddRemoveCameraTarget(mAeroplane.get(), aics.mIncludeInCameraViews && mAIControllerSetting.mIncludeInCameraViews);

    mWaypointTimer -= deltaTime;

    Vector3 deltaToTarget = mTargetWaypoint - pos;
    Vector3 horDeltaToTarget(deltaToTarget.x, deltaToTarget.y, 0.0f);
    CheckForNewWaypoint(aies, aias, pos, windVel, deltaToTarget, horDeltaToTarget);

    const float heading = atan2f(vel.x, vel.y);
    const float desiredHeading = atan2f(horDeltaToTarget.x, horDeltaToTarget.y);
    const float windHeading = atan2f(windVel.x, windVel.y);

    // Make sure we always turn into wind
    float headingChange = WrapToRange(desiredHeading - heading, -PI, PI);
    float tol = PI - windVel.GetLength() / 5.0f * PI;
    if (fabsf(headingChange) > tol)
    {
        const float headingRelWind = WrapToRange(heading - windHeading, 0.0f, 2.0f * PI);
        const float desiredHeadingRelWind = WrapToRange(desiredHeading - windHeading, 0.0f, 2.0f * PI);
        headingChange = desiredHeadingRelWind - headingRelWind;
    }
    float desiredRoll = ClampToRange(headingChange * aias.mControlBankAnglePerHeadingChange, 
        -DegreesToRadians(aias.mControlMaxBankAngle), DegreesToRadians(aias.mControlMaxBankAngle));
    const float currentRoll = asinf(tm.RowY().z);

    if (altitude < physicsRadius)
    {
        desiredRoll *= altitude / physicsRadius;
    }

    float rollControl = ClampToRange(RadiansToDegrees(desiredRoll - currentRoll) * aias.mControlRollControlPerRollAngle, -1.0f, 1.0f);
    rollControl *= aias.mControlMaxRollControl;

    // Control pitch. Note that +ve is down
    float pitchControl = RadiansToDegrees(fabsf(currentRoll)) * -aias.mControlPitchControlPerRollAngle;

    // Also adjust depending on air speed.
    float fwdSpeedRelAir = velRelWind.Dot(tm.RowX());

    // Make it so if we're going too fast, the target glide slope is positive - negative if going too slow. +ve glide slope is up.
    float currentGlideSlope = velRelWind.z / fwdSpeedRelAir;

    float adjustForAltitudeScale = ClampToRange(
        1.0f - fabsf(headingChange / DegreesToRadians(aias.mGliderControlHeadingChangeForNoSlope)), 0.0f, 1.0f);
    float targetSpeed = Maximum(aias.mGliderControlCruiseSpeed +
        (pos.z - mTargetWaypoint.z) * aias.mGliderControlSpeedPerAltitudeChange * adjustForAltitudeScale,
        aias.mGliderControlMinSpeed);
    float targetGlideSlope = ClampToRange((fwdSpeedRelAir - targetSpeed) * aias.mGliderControlSlopePerExcessSpeed, -1.0f, 1.0f);

    pitchControl -= (targetGlideSlope - currentGlideSlope) * aias.mGliderControlPitchControlPerGlideSlope;
    pitchControl = ClampToRange(pitchControl, -1.0f, 1.0f);
    pitchControl *= aias.mControlMaxPitchControl;

    mOutputControls[Controller::CHANNEL_AILERONS] = ExponentialApproach(mOutputControls[0], rollControl, deltaTime, aias.mControlRollTimeScale);
    mOutputControls[Controller::CHANNEL_ELEVATOR] = ExponentialApproach(mOutputControls[1], pitchControl, deltaTime, aias.mControlPitchTimeScale);

    mOutputControls[Controller::CHANNEL_THROTTLE] = 0.0f; // For air brakes
    mOutputControls[Controller::CHANNEL_SMOKE1]   = -1.0f; 
    mOutputControls[Controller::CHANNEL_SMOKE2]   = -1.0f;
    mOutputControls[Controller::CHANNEL_HOOK]     = -1.0f;

    if (
        aics.mEnableDebugDraw &&
        mAIControllerSetting.mEnableDebugDraw
        )
    {
        RenderManager::GetInstance().GetDebugRenderer().DrawPoint(mTargetWaypoint, 3.0f, Vector3(1, 0, 0));
        RenderManager::GetInstance().GetDebugRenderer().DrawLine(pos, mTargetWaypoint, Vector3(1, 1, 1));
        float arrowLength = mAeroplane->GetGraphics()->GetRenderBoundingRadius();
        RenderManager::GetInstance().GetDebugRenderer().DrawArrow(pos, pos + horDeltaToTarget.GetNormalised() * arrowLength, Vector3(0, 0, 0));

        if (
            PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTarget() == mAeroplane.get() ||
            PicaSim::GetInstance().GetMainViewport().GetCamera()->GetCameraTransform() == mAeroplane.get()
            )
        {
            char txt[256];
            float startY = 0.25f;
            float deltaY = 0.05f;
            float x = 0.05f;
            int iY = 0;
            sprintf(txt, "altitude = %5.1fm", altitude);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "headingChange = %5.1f deg", RadiansToDegrees(headingChange));
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "desiredRoll = %5.1f deg", RadiansToDegrees(desiredRoll));
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "fwdSpeedRelAir = %5.2f", fwdSpeedRelAir);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "rollControl = %5.2f", rollControl);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "pitchControl = %5.2f", pitchControl);
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
            sprintf(txt, "distance = %5.1fm", horDeltaToTarget.GetLength());
            RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(x, startY + iY++ * deltaY), Vector3(1,1,1));
        }
    }
}
