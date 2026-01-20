#include "PropellerEngine.h"

#include "Aerofoil.h"
#include "AerofoilParameters.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Controller.h"
#include "Environment.h"
#include "DimensionalScaling.h"
#include "PicaSim.h"

//======================================================================================================================
PropellerEngine::PropellerEngine()
{
}

//======================================================================================================================
void PropellerEngine::Init(class TiXmlElement* engineElement, class Aeroplane* aeroplane)
{
    TRACE_METHOD_ONLY(1);
    Engine::Init(engineElement, aeroplane);

    mAeroplane = aeroplane;
    const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();
    // If true, then we would adapt with the natural flight speed of the plane.
    // If false, then we seem to achieve the same flight speedirrespective of the 
    /// mass - but that makes us massively over powered (e.g. fly the quad) - but we can scale that back
    DimensionalScaling ds(as.mSizeScale, as.mMassScale, false);

    for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
    {
        mControlPerChannel[i] = 0.0f;
        mPitchAnglePerChannel[i] = 0.0f;
        mYawAnglePerChannel[i] = 0.0f;
    }
    for (size_t i = 0 ; i != MAX_GYROS ; ++i)
        mGyroControls[i] = GyroControl();
    for (size_t i = 0 ; i != MAX_ACCELEROMETERS ; ++i)
        mAccelerometerControls[i] = AccelerometerControl();

    mNumBlades = 2;
    mNumRings = 4;
    mRadius = 0.1905f;
    mPitch = 0.18f;
    mBladeChord = 0.015f;
    mCL0 = 0.5f;
    mCLPerRadian = RadiansToDegrees(0.1f);
    mCD0 = 0.05f;
    mCDInducedMultiplier = 2.0f;
    mStallAngle = 10.0f;
    mInertia = 0.001f;
    mMaxTorque = 10.0f;
    mMaxW = 0.0f;
    mMinW = 0.0f;
    mFrictionTorque = 0.0f;
    mWashRotationFraction = 0.0f;
    mAeroTorqueScale = 1.0f;
    mPropDiskAreaScale = 0.4f;
    mDirectionMultiplier = 1.0f;
    mTransitionalLiftSpeed = 5.0f;
    mTransitionalLiftAmount = 0.15f;
    mPropPredictionTime = 0.0f;
    mPropPredictionMaxAngSpeed = 3.0f;
    mIsVariable = false;

    mControlExp = 1.0f;
    // Assume a very high control rate, since this would normally be electronic, or only a small load even if using a real throttle.
    mControlRate = 1.0f / 0.05f;
    mChannelForMode = -1;

    // Read data for this engine
    mName = readStringFromXML(engineElement, "name");
    readFromXML(engineElement, "controlPerChannel0", mControlPerChannel[0]);
    readFromXML(engineElement, "controlPerChannel1", mControlPerChannel[1]);
    readFromXML(engineElement, "controlPerChannel2", mControlPerChannel[2]);
    readFromXML(engineElement, "controlPerChannel3", mControlPerChannel[3]);
    readFromXML(engineElement, "controlPerChannel4", mControlPerChannel[4]);
    readFromXML(engineElement, "controlPerChannel5", mControlPerChannel[5]);
    readFromXML(engineElement, "controlPerChannel6", mControlPerChannel[6]);
    readFromXML(engineElement, "pitchAnglePerChannel0", mPitchAnglePerChannel[0]);
    readFromXML(engineElement, "pitchAnglePerChannel1", mPitchAnglePerChannel[1]);
    readFromXML(engineElement, "pitchAnglePerChannel2", mPitchAnglePerChannel[2]);
    readFromXML(engineElement, "pitchAnglePerChannel3", mPitchAnglePerChannel[3]);
    readFromXML(engineElement, "pitchAnglePerChannel4", mPitchAnglePerChannel[4]);
    readFromXML(engineElement, "pitchAnglePerChannel5", mPitchAnglePerChannel[5]);
    readFromXML(engineElement, "pitchAnglePerChannel6", mPitchAnglePerChannel[6]);
    readFromXML(engineElement, "yawAnglePerChannel0", mYawAnglePerChannel[0]);
    readFromXML(engineElement, "yawAnglePerChannel1", mYawAnglePerChannel[1]);
    readFromXML(engineElement, "yawAnglePerChannel2", mYawAnglePerChannel[2]);
    readFromXML(engineElement, "yawAnglePerChannel3", mYawAnglePerChannel[3]);
    readFromXML(engineElement, "yawAnglePerChannel4", mYawAnglePerChannel[4]);
    readFromXML(engineElement, "yawAnglePerChannel5", mYawAnglePerChannel[5]);
    readFromXML(engineElement, "yawAnglePerChannel6", mYawAnglePerChannel[6]);
    readFromXML(engineElement, "numBlades", mNumBlades);
    readFromXML(engineElement, "numRings", mNumRings);
    readFromXML(engineElement, "radius", mRadius);
    readFromXML(engineElement, "pitch", mPitch);
    readFromXML(engineElement, "bladeChord", mBladeChord);
    readFromXML(engineElement, "CL0", mCL0);
    readFromXML(engineElement, "CLPerDegree", mCLPerRadian);
    readFromXML(engineElement, "CD0", mCD0);
    readFromXML(engineElement, "CDInducedMultiplier", mCDInducedMultiplier);
    readFromXML(engineElement, "stallAngle", mStallAngle);
    readFromXML(engineElement, "inertia", mInertia);
    readFromXML(engineElement, "maxTorque", mMaxTorque);
    readFromXML(engineElement, "maxRPM", mMaxW);
    readFromXML(engineElement, "minRPM", mMinW);
    readFromXML(engineElement, "frictionTorque", mFrictionTorque);
    readFromXML(engineElement, "aeroTorqueScale", mAeroTorqueScale);
    readFromXML(engineElement, "washRotationFraction", mWashRotationFraction);
    readFromXML(engineElement, "propDiskAreaScale", mPropDiskAreaScale);
    readFromXML(engineElement, "transitionalLiftAmount", mTransitionalLiftAmount);
    readFromXML(engineElement, "transitionalLiftSpeed", mTransitionalLiftSpeed);
    readFromXML(engineElement, "propPredictionTime", mPropPredictionTime);
    readFromXML(engineElement, "propPredictionMaxAngSpeed", mPropPredictionMaxAngSpeed);
    readFromXML(engineElement, "isVariable", mIsVariable);

    readFromXML(engineElement, "controlExp", mControlExp);
    readFromXML(engineElement, "controlRate", mControlRate);
    readFromXML(engineElement, "channelForMode", mChannelForMode);

    for (int iGyro = 0 ; iGyro != MAX_GYROS ; ++iGyro)
    {
        char txt[128];
        sprintf(txt, "gyroName%d", iGyro);
        mGyroControls[iGyro].mName = readStringFromXML(engineElement, txt);
        sprintf(txt, "controlPerGyro%d", iGyro);
        mGyroControls[iGyro].mControl = readFloatFromXML(engineElement, txt);
    }
    for (int iAccelerometer = 0 ; iAccelerometer != MAX_ACCELEROMETERS ; ++iAccelerometer)
    {
        char txt[128];
        sprintf(txt, "accelerometerName%d", iAccelerometer);
        mAccelerometerControls[iAccelerometer].mName = readStringFromXML(engineElement, txt);
        sprintf(txt, "controlPerAccelerometer%d", iAccelerometer);
        mAccelerometerControls[iAccelerometer].mControl = readFloatFromXML(engineElement, txt);
    }

    mCLPerRadian = RadiansToDegrees(mCLPerRadian);
    mMaxW *= 2.0f * PI / 60.0f;
    mMinW *= 2.0f * PI / 60.0f;

    if (mMaxW < 0.0f)
    {
        mMaxW *= -1.0f;
        mDirectionMultiplier = -1.0f;
        mMinW *= -1.0f;
    }

    // Scale
    ds.ScaleLength(mRadius);
    ds.ScaleLength(mPitch);
    ds.ScaleLength(mBladeChord);
    ds.ScaleInertia(mInertia);
    ds.ScaleTorque(mMaxTorque);
    mMaxW = ds.GetScaledAngVel(mMaxW) * as.mEngineScale;;
    mMinW = ds.GetScaledAngVel(mMinW) * as.mEngineScale;;
    ds.ScaleTorque(mFrictionTorque);
    ds.ScaleVel(mTransitionalLiftSpeed);
    ds.ScaleTime(mPropPredictionTime);
    ds.ScaleAngVel(mPropPredictionMaxAngSpeed);

    // Need to adjust for the mass scaling - we simply need to generate less thrust when the plane is lighter...
    mMaxW *= sqrtf(as.mMassScale);
    mMinW *= sqrtf(as.mMassScale);

    Vector3 enginePosition = ds.GetScaledLength(readVector3FromXML(engineElement, "position"));
    Vector3 engineRotation = readVector3FromXML(engineElement, "rotation");

    float roll = readFloatFromXML(engineElement, "roll");
    float pitch = readFloatFromXML(engineElement, "pitch");
    float yaw = readFloatFromXML(engineElement, "yaw");
    ApplyRollPitchYawToRotationDegrees(roll, pitch, yaw, engineRotation);

    float angle = DegreesToRadians(engineRotation.GetLength());
    if (angle > 0.0f)
        engineRotation.Normalise();
    else
        engineRotation = Vector3(1,0,0);

    // Store the info about the engine
    mTMLocal.SetAxisAngle(engineRotation, angle);
    mTMLocal.SetTrans(enginePosition);

    // Calculated values
    mPropSolidity = 2.0f * mBladeChord * mRadius / (PI * mRadius * mRadius);

    // Initial values
    Launched();

    std::string audioFile = readStringFromXML(engineElement, "audioFile");
    if (!audioFile.empty())
    {
        SoundSetting soundSetting;

        int sampleRate = readIntFromXML(engineElement, "audioSampleRate");
        float radius = readFloatFromXML(engineElement, "audioRadius");
        soundSetting.mSound = AudioManager::GetInstance().LoadSound(audioFile.c_str(), sampleRate, false, true, true);
        if (soundSetting.mSound)
        {
            soundSetting.mSoundChannel = AudioManager::GetInstance().AllocateSoundChannel(radius, true);
            if (soundSetting.mSoundChannel != -1)
            {
                AudioManager::GetInstance().StartSoundOnChannel(soundSetting.mSoundChannel, soundSetting.mSound, true);
                soundSetting.mMinVolume = readFloatFromXML(engineElement, "audioMinVolume");
                soundSetting.mMaxVolume = readFloatFromXML(engineElement, "audioMaxVolume");
                soundSetting.mMinFreqScale = readFloatFromXML(engineElement, "audioMinFreqScale");
                soundSetting.mMaxFreqScale = readFloatFromXML(engineElement, "audioMaxFreqScale");
                ds.ScaleFreq(soundSetting.mMinFreqScale);
                ds.ScaleFreq(soundSetting.mMaxFreqScale);
                mSoundSettings.push_back(soundSetting);
            }
            else
            {
                AudioManager::GetInstance().UnloadSound(soundSetting.mSound);
            }
        }
    }

    EntityManager::GetInstance().RegisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//======================================================================================================================
void PropellerEngine::Terminate()
{
    TRACE_METHOD_ONLY(1);
    for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
    {
        if (mSoundSettings[i].mSoundChannel != -1)
            AudioManager::GetInstance().ReleaseSoundChannel(mSoundSettings[i].mSoundChannel);
        if (mSoundSettings[i].mSound)
            AudioManager::GetInstance().UnloadSound(mSoundSettings[i].mSound);
    }
    mSoundSettings.clear();

    EntityManager::GetInstance().UnregisterEntity(this, ENTITY_LEVEL_POST_PHYSICS);
}

//======================================================================================================================
void PropellerEngine::Launched()
{
    mTM = mTMLocal * mAeroplane->GetTransform();
    mVel = Vector3(0,0,0);
    mLastWash = Vector3(0,0,0);
    mLastWashAngVel = Vector3(0,0,0);
    mControl = 0.0f;
    mW = 0.0f;
    mAngle = 0.0f;
    mMode = MODE_THROTTLE;
}

//======================================================================================================================
void PropellerEngine::UpdatePrePhysics(float deltaTime, const TurbulenceData& turbulenceData)
{
    if (mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_PROPELLER))
    {
        mLastWash = Vector3(0,0,0);
        mLastWashAngVel = Vector3(0,0,0);
        mW = 0.0f;
        mControl = 0.0f;
        return;
    }

    // Update the control
    float throttleControl = 0.0f;
    float pitchControl = 0.0;

    float pitchAngle = 0.0f;
    float yawAngle = 0.0f;

    {
        const Controller& controller = mAeroplane->GetController();

        if (mChannelForMode != -1)
        {
            float input = controller.GetControl((Controller::Channel) mChannelForMode);
            if (input < -0.5f)
                mMode = MODE_THROTTLE;
            else
                mMode = MODE_VPP;
        }

        float control = 0.0f;
        for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
        {
            float channelControl = controller.GetControl((Controller::Channel) i);
            control += ClampToRange(mControlPerChannel[i] * channelControl, -1.0f, 1.0f);
            pitchAngle += mPitchAnglePerChannel[i] * channelControl;
            yawAngle += mYawAnglePerChannel[i] * channelControl;
        }
        pitchAngle = DegreesToRadians(pitchAngle);
        yawAngle = DegreesToRadians(yawAngle);

        for (size_t i = 0 ; i != MAX_GYROS ; ++i)
        {
            const GyroControl& gyroControl = mGyroControls[i];
            if (gyroControl.mControl != 0.0f)
            {
                const Gyro* gyro = mAeroplane->GetPhysics()->GetGyro(gyroControl.mName);
                IwAssert(ROWLHOUSE, gyro);
                if (gyro)
                    control += ClampToRange(gyroControl.mControl * gyro->GetOutput(), -1.0f, 1.0f);
            }
        }
        for (size_t i = 0 ; i != MAX_ACCELEROMETERS ; ++i)
        {
            const AccelerometerControl& accelerometerControl = mAccelerometerControls[i];
            if (accelerometerControl.mControl != 0.0f)
            {
                const Accelerometer* accelerometer = mAeroplane->GetPhysics()->GetAccelerometer(accelerometerControl.mName);
                IwAssert(ROWLHOUSE, accelerometer);
                if (accelerometer)
                    control += ClampToRange(accelerometerControl.mControl * accelerometer->GetOutput(), -1.0f, 1.0f);
            }
        }

        if (mMode == MODE_THROTTLE)
        {
            pitchAngle = yawAngle = 0.0f;

            // Engine control needs to be remapped, and can never be negative!
            control = ClampToRange(control, 0.0f, 1.0f);
            control = powf(control, mControlExp);

            float maxDeltaControl = mControlRate * deltaTime;
            if (control > mControl)
                mControl += Minimum(control - mControl, maxDeltaControl);
            else 
                mControl -= Minimum(mControl - control, maxDeltaControl);

            throttleControl = mControl;
            pitchControl = 0.0;
        }
        else
        {
            pitchControl = ClampToRange(control * 2.0f - 1.0f, -1.0f, 1.0f); 
            control = fabsf(pitchControl);
            control = powf(control, mControlExp);

            float maxDeltaControl = mControlRate * deltaTime;
            if (control > mControl)
                mControl += Minimum(control - mControl, maxDeltaControl);
            else 
                mControl -= Minimum(mControl - control, maxDeltaControl);

            throttleControl = mControl;

        }
    }

    // These are the forces on the prop
    float propAeroForce = 0.0f;
    float propAeroTorque = 0.0f;

    Vector3 thrustDir = mTM.RowX();
    if (pitchAngle != 0.0f || yawAngle != 0.0f)
    {
        Vector3 rotation = mTM.RowY() * pitchAngle + mTM.RowZ() * yawAngle;
        float angle = rotation.GetLength();
        Quat q(rotation / angle, -angle);
        thrustDir = q.RotateVector(thrustDir);
    }

    // Absolute speed of the propeller
    float propForwardSpeed = mVel.Dot(thrustDir);
    // Absolute air speed along the propeller
    Vector3 wind = Environment::GetInstance().GetWindAtPosition(
        mTM.GetTrans(), Environment::WIND_TYPE_SMOOTH | Environment::WIND_TYPE_GUSTY);

    Vector3 velRelWind = mVel - wind;
    float speedRelWind = velRelWind.GetLength();
    float washFrac = ClampToRange(1.0f - (speedRelWind / mTransitionalLiftSpeed), 0.0f, 1.0f);
    wind += mLastWash * mTransitionalLiftAmount * washFrac;
    float windForwardSpeed = wind.Dot(thrustDir);
    // Incoming air speed relative to the propeller
    float v0 = propForwardSpeed - windForwardSpeed;
    const float airDensity = mAeroplane->GetAirDensity();

    float aspectRatio = mRadius / mBladeChord;
    // Make the calculations at a number of rings
    int numRings = 4;
    float dR = mRadius / numRings;

    const float stallAngleStartHigh = DegreesToRadians(mStallAngle);
    const float stallAngleStartLow = DegreesToRadians(-mStallAngle);
    const float stallAngleRange = DegreesToRadians(5.0f);
    for (int iRing = 0 ; iRing != numRings ; ++iRing)
    {
        const float r = (0.5f + iRing) * mRadius / numRings;

        // Angle of the blade
        float bladeAngle = 
            mIsVariable ? 
            atan2f(mPitch, 2.0f * PI * mRadius) :
            atan2f(mPitch, 2.0f * PI * r);
        if (mIsVariable && mMode == MODE_VPP)
            bladeAngle *= pitchControl;

        const float bladeSpeed = mW * r;
        const float airSpeedSq = Square(bladeSpeed) + Square(v0);

        // Angle of the air flow so 0 means the lift will be straight forwards relative to the plane. 
        // If the blades were stationary but the plane going forwards, angle would be -ve.
        const float airflowAngle = -atan2f(v0, bladeSpeed);

        // Angle of attack at the blade
        const float alpha = bladeAngle + airflowAngle;

        float CL = mCL0 + mCLPerRadian * alpha;
        const float CDInduced = CL * CL * mCDInducedMultiplier / (PI * aspectRatio);
        float CD = mCD0 + CDInduced;

        // Handle prop stalling
        float separated = 0.0f;
        if (alpha > stallAngleStartHigh)
            separated = Minimum((alpha - stallAngleStartHigh) / stallAngleRange, 1.0f);
        else if (alpha < stallAngleStartLow)
            separated = Minimum(-(alpha - stallAngleStartLow) / stallAngleRange, 1.0f);
        if (separated != 0.0f)
        {
            // stalled
            const float CLFortyFive = 1.1f;
            const float CDStalled = 1.5f;
            const float sinTwiceAngleOfAttack = FastSin(2.0f * alpha);
            const float sinAngleOfAttack = FastSin(alpha);
            CL = CL * (1.0f - separated) + separated * CLFortyFive * sinTwiceAngleOfAttack;
            CD = CD * (1.0f - separated) + separated * fabsf(CDStalled * sinAngleOfAttack);

            if (PicaSim::GetInstance().GetSettings().mOptions.mStallMarkers)
            {
                const Vector3 colour(separated, 1.0f - separated, 0.0f);
                RenderManager::GetInstance().GetDebugRenderer().DrawCircle(
                    mTM.GetTrans(), thrustDir, r, colour);
            }
        }

        const float dynamicPressure = 0.5f * airDensity * airSpeedSq;

        const float effectiveArea = mBladeChord * dR;
        const float liftForce = effectiveArea * dynamicPressure * CL;
        const float dragForce = effectiveArea * dynamicPressure * CD;

    // Note that the lift and drag forces are relative to the airflow, not the prop.
        const float cosA = FastCos(airflowAngle);
        const float sinA = FastSin(airflowAngle);

        // Force in the direction of the prop axis
        const float drivingForce = liftForce * cosA + dragForce * sinA;
        const float spinForce = -dragForce * cosA + liftForce * sinA;

        // Force and torque on the prop
        propAeroForce += drivingForce;
        propAeroTorque -= spinForce * r;
    }

    propAeroForce *= mNumBlades;
    propAeroTorque *= mNumBlades;

    // Prop wash. See Selig and Waqas Khan papers.
    // V0 is propeller forward speed in still air (or, airflow speed at infinity)
    // w is induced flow speed at the prop disk
    // A = prop area
    // Momentum theory gives T = 2 * density * A * (w + V0) * V0.
    // Solve as a quadratic to give V0:
    // w = 0.5 * (-v0 +/- sqrt(v0^2 + 2 * T / (density * A)))
    if (propAeroForce != 0.0f)
    {
        // Note that v0 has the opposite sign to propAeroForce here - i.e. is +ve when the wind 
        // is blowing back through the propeller.
        float propArea = PI * Square(mRadius);
        bool reverseFlow = propAeroForce < 0.0f;
        if (reverseFlow)
        {
            propAeroForce *= -1.0f;
            v0 *= -1.0f;
        }
        float term = Square(v0) + 2.0f * propAeroForce / (airDensity * propArea);
        float washSpeed = 0.5f * (-v0 + sqrtf(term));
        if (reverseFlow)
        {
            washSpeed *= -1.0f;
            propAeroForce *= -1.0f;
            v0 *= -1.0f;
        }
        mLastWash = thrustDir * -washSpeed;
        mLastWashAngVel = thrustDir * mW * mWashRotationFraction;
    }
    else
    {
        mLastWash = Vector3(0,0,0);
        mLastWashAngVel = Vector3(0,0,0);
    }

    // Update the propeller speed
    float engineTorque = 0.0f;
    const float speedFrac = mW / mMaxW;
    if (speedFrac < throttleControl)
    {
        float delta = ClampToRange(throttleControl - speedFrac, -1.0f, 1.0f);
        float gain = 5.0f;
        engineTorque = mMaxTorque * delta * gain;
    }

    float frictionTorque = mFrictionTorque;
    if (deltaTime > 0.0f)
    {
        // Avoid friction reversing the direction and oscillating
        float maxFrictionTorque = fabsf(mW) * mInertia / deltaTime;
        if (mFrictionTorque > maxFrictionTorque)
            frictionTorque = maxFrictionTorque;

        // Avoid overshooting
        if (engineTorque > 0.0f)
        {
            float requiredAngVelIncrease = throttleControl * mMaxW - mW;
            float maxEngineTorque = requiredAngVelIncrease * mInertia / deltaTime;
            if (engineTorque > maxEngineTorque)
                engineTorque = maxEngineTorque;
        }
    }

    // speedFrac can be -ve 
    engineTorque += speedFrac > 0.0f ? -frictionTorque : frictionTorque;

    float propTorque = engineTorque - propAeroTorque;

    float angAccel = propTorque / mInertia;
    mW += angAccel * deltaTime;

    if (mW < mMinW)
        mW = mMinW;

    // Accumulate the angle - not used for physics
    float deltaAngle = mW * deltaTime;
    mAngle += deltaAngle;

    Vector3 force = thrustDir * propAeroForce;

    // Use a predicted direction for the thrust. I have no idea why, but this results 
    // in the desired instability when pushing...
    Vector3 angVel = mAeroplane->GetPhysics()->GetAngularVelocity();
    float angSpeed = angVel.GetLength();
    if (angSpeed * mPropPredictionTime * mPropPredictionMaxAngSpeed > 0.0f)
    {
        angVel *= 1.0f/angSpeed;
        angSpeed = ClampToRange(angSpeed, -mPropPredictionMaxAngSpeed, mPropPredictionMaxAngSpeed);

        Quat q(angVel, -angSpeed * mPropPredictionTime);
        force = q.RotateVector(force);
    }

    // Apply the forces
    mAeroplane->GetPhysics()->ApplyWorldForceAtWorldPosition(force,  mTM.GetTrans());
    mAeroplane->GetPhysics()->ApplyWorldTorque(thrustDir * (-engineTorque * mDirectionMultiplier * mAeroTorqueScale));

    // Roll and pitch damping
    if (fabsf(mW) > 0.01f)
    {
        Vector3 planeAngVel = mAeroplane->GetAngularVelocity();
        planeAngVel -= thrustDir * planeAngVel.Dot(thrustDir);

        float kd = 2.0f * PI * PI * mPropSolidity;
        float scale = -kd * airDensity * 0.5f * Square(mW) * (mRadius*mRadius*mRadius*mRadius*mRadius);
        Vector3 dampingMoment;
        dampingMoment.x = scale * atanf(planeAngVel.x / mW);
        dampingMoment.y = scale * atanf(planeAngVel.y / mW);
        dampingMoment.z = scale * atanf(planeAngVel.z / mW);

        mAeroplane->GetPhysics()->ApplyWorldTorque(dampingMoment);
    }

    // Also approximate the drag due to air flow in the plane of the prop.
    Vector3 tangentVelRelWind = velRelWind - velRelWind.Dot(thrustDir) * thrustDir;
    float tangentSpeed = tangentVelRelWind.GetLength();
    {
        float kj = mPropDiskAreaScale;
        float area = PI * Square(mRadius);
        float T = fabsf(propAeroForce);
        float w0 = sqrtf(T / (2.0f * airDensity * area));
        float scale = kj * airDensity * area * w0;
        Vector3 dragForce = -scale * tangentVelRelWind;
        mAeroplane->GetPhysics()->ApplyWorldForceAtWorldPosition(dragForce, mTM.GetTrans());
    }
    // Inform physics of the angular momentum for gyroscopic precession
    float angMom = mW * mInertia;
    Vector3 extraAngularMomentum = thrustDir * angMom;
    mAeroplane->GetPhysics()->AddExtraAngularMomentum(extraAngularMomentum * mDirectionMultiplier);
}

//======================================================================================================================
void PropellerEngine::UpdatePostPhysics(float deltaTime)
{
    mTM = mTMLocal * mAeroplane->GetPhysics()->GetTransform();
    mVel = mAeroplane->GetPhysics()->GetVelAtPoint(mTM.GetTrans());
}

//======================================================================================================================
void PropellerEngine::EntityUpdate(float deltaTime, int entityLevel)
{
    for (size_t i = 0 ; i != mSoundSettings.size() ; ++i)
    {
        const SoundSetting& soundSetting = mSoundSettings[i];
        if (!soundSetting.mSound || soundSetting.mSoundChannel == -1)
            continue;

        float speed = mW / mMaxW;

        float freqScale = soundSetting.mMinFreqScale + speed * (soundSetting.mMaxFreqScale - soundSetting.mMinFreqScale);
        float volumeScale = soundSetting.mMinVolume + speed * (soundSetting.mMaxVolume - soundSetting.mMinVolume);

        if (PicaSim::GetInstance().GetStatus() == PicaSim::STATUS_PAUSED)
            volumeScale = 0.0f;

        freqScale *= PicaSim::GetInstance().GetTimeScale();

        AudioManager::GetInstance().SetChannelPositionAndVelocity(soundSetting.mSoundChannel, mTM.GetTrans(), mVel);
        AudioManager::GetInstance().SetChannelFrequencyScale(soundSetting.mSoundChannel, freqScale);
        if (PicaSim::GetInstance().GetMode() == PicaSim::MODE_GROUND)
            AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mOutsideAeroplaneVolume * volumeScale);
        else
            AudioManager::GetInstance().SetChannelTargetVolumeScale(soundSetting.mSoundChannel, PicaSim::GetInstance().GetSettings().mOptions.mInsideAeroplaneVolume * volumeScale);

        if (mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_PROPELLER))
            AudioManager::GetInstance().SetChannelVolumeScale(soundSetting.mSoundChannel, 0.0f);
    }
}
