#include "Wing.h"

#include "Aerofoil.h"
#include "AerofoilParameters.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Controller.h"
#include "DimensionalScaling.h"
#include "PicaSim.h"

// For data that needs to be interpreted for the aerofoil
struct AerofoilData
{
  AerofoilData() 
    : 
    numSections(1), numPieces(1), wingPosition(0,0,0), wingRotation(0,0,0), wingExtents(1,1,1), shadow(0,0,0),
    wingAspectRatio(1), wingSpanEfficiency(0.9f), controlHalfSpeed(100),
    roll(0), pitch(0), yaw(0), groundEffect(false), washFromEngineFraction(0.0f) 
  {
    for (size_t i = 0 ; i != 4 ; ++i)
      washFromWingFraction[i] = 0.0f;
  }
  std::string aerofoilName;
  int     numSections; ///< Number of spanwise sections
  int     numPieces;   ///< Number of chordwise pieces (for fuselages)
  // Note that the wing position is mid-height and mid-chord, but at one end
  Vector3 wingPosition;
  Vector3 wingRotation;
  Vector3 wingExtents;
  Vector3 shadow;
  float wingAspectRatio;
  float wingSpanEfficiency;
  float controlHalfSpeed;
  float roll;
  float pitch;
  float yaw;
  bool groundEffect;

  std::string washFromEngineName;
  float washFromEngineFraction;
  std::string washFromWingName[4];
  float washFromWingFraction[4];
};

//----------------------------------------------------------------------------------------------------------------------
bool Wing::ReadFromXML(class TiXmlElement* wingElement, AerofoilData& aerofoilData)
{
  mName = readStringFromXML(wingElement, "name");
  readFromXML(wingElement, "mass", mMass); mMass = Maximum(mMass, 1e-8f);
  readFromXML(wingElement, "collide", mCollide);
  readFromXML(wingElement, "isMainWing", mIsMainWing);
  readFromXML(wingElement, "controlPerChannel0", mControlPerChannel[0]);
  readFromXML(wingElement, "controlPerChannel1", mControlPerChannel[1]);
  readFromXML(wingElement, "controlPerChannel2", mControlPerChannel[2]);
  readFromXML(wingElement, "controlPerChannel3", mControlPerChannel[3]);
  readFromXML(wingElement, "controlPerChannel4", mControlPerChannel[4]);
  readFromXML(wingElement, "controlPerChannel5", mControlPerChannel[5]);
  readFromXML(wingElement, "controlPerChannel6", mControlPerChannel[6]);
  readFromXML(wingElement, "controlDifferentialPerChannel0", mControlDifferentialPerChannel[0]);
  readFromXML(wingElement, "controlDifferentialPerChannel1", mControlDifferentialPerChannel[1]);
  readFromXML(wingElement, "controlDifferentialPerChannel2", mControlDifferentialPerChannel[2]);
  readFromXML(wingElement, "controlDifferentialPerChannel3", mControlDifferentialPerChannel[3]);
  readFromXML(wingElement, "controlDifferentialPerChannel4", mControlDifferentialPerChannel[4]);
  readFromXML(wingElement, "controlDifferentialPerChannel5", mControlDifferentialPerChannel[5]);
  readFromXML(wingElement, "controlDifferentialPerChannel6", mControlDifferentialPerChannel[6]);
  readFromXML(wingElement, "controlExpPerChannel0", mControlExpPerChannel[0]);
  readFromXML(wingElement, "controlExpPerChannel1", mControlExpPerChannel[1]);
  readFromXML(wingElement, "controlExpPerChannel2", mControlExpPerChannel[2]);
  readFromXML(wingElement, "controlExpPerChannel3", mControlExpPerChannel[3]);
  readFromXML(wingElement, "controlExpPerChannel4", mControlExpPerChannel[4]);
  readFromXML(wingElement, "controlExpPerChannel5", mControlExpPerChannel[5]);
  readFromXML(wingElement, "controlExpPerChannel6", mControlExpPerChannel[6]);
  readFromXML(wingElement, "controlPerAbsChannel0", mControlPerAbsChannel[0]);
  readFromXML(wingElement, "controlPerAbsChannel1", mControlPerAbsChannel[1]);
  readFromXML(wingElement, "controlPerAbsChannel2", mControlPerAbsChannel[2]);
  readFromXML(wingElement, "controlPerAbsChannel3", mControlPerAbsChannel[3]);
  readFromXML(wingElement, "controlPerAbsChannel4", mControlPerAbsChannel[4]);
  readFromXML(wingElement, "controlPerAbsChannel5", mControlPerAbsChannel[5]);
  readFromXML(wingElement, "controlPerAbsChannel6", mControlPerAbsChannel[6]);
  readFromXML(wingElement, "controlRate", mControlRate);
  readFromXML(wingElement, "controlClamp", mControlClamp);
  readFromXML(wingElement, "trimControl", mTrimControl);
  readFromXML(wingElement, "CLPerDegree", mCLPerDegree);
  readFromXML(wingElement, "CDPerDegree", mCDPerDegree);
  readFromXML(wingElement, "CMPerDegree", mCMPerDegree);
  readFromXML(wingElement, "degreesPerControl", mDegreesPerControl);
  readFromXML(wingElement, "flapFraction", mFlapFraction);
  readFromXML(wingElement, "slopDegreesPerNewton", mSlopDegreesPerNewton);


  // Read data used to set up each aerofoil
  readFromXML(wingElement, "aerofoil", aerofoilData.aerofoilName);
  readFromXML(wingElement, "numSections", aerofoilData.numSections);
  readFromXML(wingElement, "numPieces", aerofoilData.numPieces);
  readFromXML(wingElement, "position", aerofoilData.wingPosition);
  readFromXML(wingElement, "rotation", aerofoilData.wingRotation);
  readFromXML(wingElement, "extents", aerofoilData.wingExtents);
  readFromXML(wingElement, "shadow", aerofoilData.shadow);
  readFromXML(wingElement, "wingAspectRatio", aerofoilData.wingAspectRatio);
  readFromXML(wingElement, "wingSpanEfficiency", aerofoilData.wingSpanEfficiency);
  readFromXML(wingElement, "controlHalfSpeed", aerofoilData.controlHalfSpeed);
  readFromXML(wingElement, "roll", aerofoilData.roll);
  readFromXML(wingElement, "pitch", aerofoilData.pitch);
  readFromXML(wingElement, "yaw", aerofoilData.yaw);
  readFromXML(wingElement, "groundEffect", aerofoilData.groundEffect);

  readFromXML(wingElement, "washFromEngineName", aerofoilData.washFromEngineName);
  readFromXML(wingElement, "washFromEngineFraction", aerofoilData.washFromEngineFraction);

  readFromXML(wingElement, "washFromWingName1", aerofoilData.washFromWingName[0]);
  readFromXML(wingElement, "washFromWingFraction1", aerofoilData.washFromWingFraction[0]);
  readFromXML(wingElement, "washFromWingName2", aerofoilData.washFromWingName[1]);
  readFromXML(wingElement, "washFromWingFraction2", aerofoilData.washFromWingFraction[1]);
  readFromXML(wingElement, "washFromWingName3", aerofoilData.washFromWingName[2]);
  readFromXML(wingElement, "washFromWingFraction3", aerofoilData.washFromWingFraction[2]);
  readFromXML(wingElement, "washFromWingName4", aerofoilData.washFromWingName[3]);
  readFromXML(wingElement, "washFromWingFraction4", aerofoilData.washFromWingFraction[3]);

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
void Wing::Init(
  TiXmlElement* wingElement, 
  TiXmlHandle& aerodynamicsHandle, 
  Aeroplane* aeroplane, uint32& checksum)
{
  TRACE_METHOD_ONLY(1);
  mAeroplane = aeroplane;
  const AeroplaneSettings& as = mAeroplane->GetAeroplaneSettings();

  // Initialise data
  for (size_t i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
  {
    mControlPerChannel[i] = mControlPerAbsChannel[i] = mControlDifferentialPerChannel[i] = 0.0f;
    mControlExpPerChannel[i] = 1.0f;
  }

  mCLPerDegree = mCDPerDegree = mCMPerDegree = mDegreesPerControl = mTrimControl = 0.0f;
  mFlapFraction = mSlopDegreesPerNewton = 0.0f;
  // Assume a default of 0.1s per 60 degrees (which we'll consider to be full control)
  mControlRate = 1.0f / 0.1f;
  mControlClamp = FLT_MAX;
  // Read data for this wing
  mCollide = true;
  mIsMainWing = false;
  mMass = 1.0f;

  AerofoilData aerofoilData;

  // Copy if required
  std::string copy = readStringFromXML(wingElement, "copy");
  if (!copy.empty())
  {
    for (int iWing = 0 ; ; ++iWing)
    {
      TiXmlElement* wingElementToCopy = aerodynamicsHandle.Child("Wing", iWing).ToElement();
      if (!wingElementToCopy)
        break;
      const std::string name = readStringFromXML(wingElementToCopy, "name");

      if (name == copy)
      {
        ReadFromXML(wingElementToCopy, aerofoilData);
        break;
      }
    }
  }


  ReadFromXML(wingElement, aerofoilData);

  // Mirror
  bool mirror = readBoolFromXML(wingElement, "mirror");
  if (mirror)
  {
    aerofoilData.wingPosition.y *= -1.0f;
    aerofoilData.wingExtents.y *= -1.0f;
    aerofoilData.wingRotation.x *= -1.0f;
    aerofoilData.wingRotation.z *= -1.0f;
    aerofoilData.shadow.y *= -1.0f;
    aerofoilData.roll *= -1.0f;
    aerofoilData.yaw *= -1.0f;
    // Flip aileron and rudder control
    mControlPerChannel[0] *= -1.0f;
    mControlPerChannel[2] *= -1.0f;
  }
  // Process the roll/pitch/yaw
  ApplyRollPitchYawToRotationDegrees(aerofoilData.roll, aerofoilData.pitch, aerofoilData.yaw, aerofoilData.wingRotation);
  float angle = DegreesToRadians(aerofoilData.wingRotation.GetLength());
  if (angle > 0.0f)
    aerofoilData.wingRotation.Normalise();
  else
    aerofoilData.wingRotation = Vector3(1,0,0);


  // Scaling
  DimensionalScaling ds(as.mSizeScale, as.mMassScale, true);
  ds.ScaleMass(mMass);
  ds.ScaleLength(aerofoilData.wingPosition);
  ds.ScaleLength(aerofoilData.wingExtents);
  mSlopDegreesPerNewton /= ds.GetScaledForce(1.0f);

  // Set up configuration for each aerofoil
  mAerofoils.reserve(aerofoilData.numSections * aerofoilData.numPieces);
  AerofoilConfiguration aerofoilConfiguration;
  aerofoilConfiguration.mOffset.SetAxisAngle(aerofoilData.wingRotation, angle);
  aerofoilConfiguration.mExtents = aerofoilData.wingExtents;
  aerofoilConfiguration.mExtents.y = fabsf(aerofoilConfiguration.mExtents.y / aerofoilData.numSections);
  aerofoilConfiguration.mExtents.x = fabsf(aerofoilConfiguration.mExtents.x / aerofoilData.numPieces);
  aerofoilConfiguration.mShadow = aerofoilData.shadow;
  aerofoilConfiguration.mArea = aerofoilConfiguration.mExtents.x * aerofoilConfiguration.mExtents.y;
  aerofoilConfiguration.mWingAspectRatio = aerofoilData.wingAspectRatio;
  aerofoilConfiguration.mWingSpanEfficiency = aerofoilData.wingSpanEfficiency;
  aerofoilConfiguration.mControlHalfSpeed = aerofoilData.controlHalfSpeed;
  aerofoilConfiguration.mGroundEffect = aerofoilData.groundEffect;

  aerofoilConfiguration.mWashFromEngineName = aerofoilData.washFromEngineName;
  aerofoilConfiguration.mWashFromEngineFraction = aerofoilData.washFromEngineFraction;
  aerofoilConfiguration.mWashFromWingName[0]     = aerofoilData.washFromWingName[0];
  aerofoilConfiguration.mWashFromWingFraction[0] = aerofoilData.washFromWingFraction[0];
  aerofoilConfiguration.mWashFromWingName[1]     = aerofoilData.washFromWingName[1];
  aerofoilConfiguration.mWashFromWingFraction[1] = aerofoilData.washFromWingFraction[1];
  aerofoilConfiguration.mWashFromWingName[2]     = aerofoilData.washFromWingName[2];
  aerofoilConfiguration.mWashFromWingFraction[2] = aerofoilData.washFromWingFraction[2];
  aerofoilConfiguration.mWashFromWingName[3]     = aerofoilData.washFromWingName[3];
  aerofoilConfiguration.mWashFromWingFraction[3] = aerofoilData.washFromWingFraction[3];

  // Store the info about the wing
  mTMLocal.SetAxisAngle(aerofoilData.wingRotation, angle);
  mTMLocal.SetTrans(aerofoilData.wingPosition + aerofoilConfiguration.mOffset.RowY() * aerofoilData.wingExtents.y * 0.5f);
  mExtents = aerofoilData.wingExtents;
  mExtents.y = fabsf(aerofoilData.wingExtents.y);

  for (int iPiece = 0 ; iPiece != aerofoilData.numPieces ; ++iPiece)
  {
    for (int iSection = 0 ; iSection != aerofoilData.numSections ; ++iSection)
    {
      float f = (iSection + 0.5f) / aerofoilData.numSections;
      float g = (iPiece + 0.5f);
      Vector3 pos = aerofoilData.wingPosition;
      pos -= aerofoilConfiguration.mOffset.RowX() * (aerofoilData.numPieces * aerofoilConfiguration.mExtents.x * 0.5f); // now at the back
      pos += aerofoilConfiguration.mOffset.RowX() * aerofoilConfiguration.mExtents.x * g;
      pos += aerofoilConfiguration.mOffset.RowY() * aerofoilData.wingExtents.y * f;
      aerofoilConfiguration.mOffset.SetTrans(pos);

      Aerofoil* aerofoil = new Aerofoil;
      aerofoil->Init(aerofoilData.aerofoilName.c_str(), aerofoilConfiguration, mAeroplane, checksum);
      mAerofoils.push_back(aerofoil);
    }
  }

  mControl = 0.0f;
  mFlapAngle = 0.0f;
  mSlopAngle = 0.0f;
  mSlopAngleRate = 0.0f;
  mLastWash = Vector3(0,0,0);
  mLastForce = Vector3(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
void Wing::Terminate()
{
  TRACE_METHOD_ONLY(1);
  while (!mAerofoils.empty())
  {
    Aerofoil* aerofoil = mAerofoils.back();
    aerofoil->Terminate();
    delete aerofoil;
    mAerofoils.pop_back();
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Wing::UpdatePrePhysics(float deltaTime, const TurbulenceData& turbulenceData)
{
  // Update the controls. Do this by making a controller object (entity) attached to the aeroplane, 
  // then we get the channel info from that, and process it for the aerofoils

  if (!mAeroplane->GetCrashed(Aeroplane::CRASHFLAG_AIRFRAME))
  {
    const Controller& controller = mAeroplane->GetController();
    float control = mTrimControl;

    bool controlled = true;
    float responsiveness = 1.0f;
    mAeroplane->GetPhysics()->GetIsControllable(controlled, responsiveness);
    if (controlled)
    {
      for (unsigned int i = 0 ; i != Controller::MAX_CHANNELS ; ++i)
      {
        float c = mControlPerChannel[i] * controller.GetControl((Controller::Channel) i) + mControlPerAbsChannel[i] * fabsf(controller.GetControl((Controller::Channel) i));
        if (c > 0.0f)
        {
          c = powf(c, mControlExpPerChannel[i]);
          c *= (1.0f + mControlDifferentialPerChannel[i]);
        }
        else
        {
          c = -powf(-c, mControlExpPerChannel[i]);
          c *= (1.0f - mControlDifferentialPerChannel[i]);
        }
        control += c;
      }
      if (fabsf(control - mControl) < 1.0f - responsiveness)
        control = mControl;

    }
    // Represent finite servo speed - allow infinite speed when paused
    {
      float dt = deltaTime > 0.0f ? deltaTime : PicaSim::GetInstance().GetCurrentUpdateDeltaTime();
      float maxDeltaControl = mControlRate * dt;
      if (control > mControl)
        mControl += Minimum(control - mControl, maxDeltaControl);
      else 
        mControl -= Minimum(mControl - control, maxDeltaControl);
    }

    mControl = ClampToRange(mControl, -mControlClamp, mControlClamp);
  }

  mFlapAngle = mControl * mDegreesPerControl;

  Vector3 upDir = mTMLocal.RowZ();
  upDir = mAeroplane->GetTransform().RotateVec(upDir);

  float slopAngleTarget = mLastForce.Dot(upDir) * -mSlopDegreesPerNewton;
  SmoothCD(mSlopAngle, mSlopAngleRate, deltaTime, slopAngleTarget, 0.01f);

  AerofoilControl aerofoilControl;
  // CL rate is constant up to a max angle;
  aerofoilControl.mExtraCL = mCLPerDegree * ClampToRange(getFlapAngle(), -60.0f, 60.0f);
  aerofoilControl.mExtraCM = mCMPerDegree * ClampToRange(getFlapAngle(), -60.0f, 60.0f);
  aerofoilControl.mExtraCamber = DegreesToRadians(getFlapAngle()) * (1.0f - Square(2.0f * mFlapFraction - 1.0f));
  aerofoilControl.mExtraCD = mCDPerDegree * 45.0f * (1.0f - FastCos(DegreesToRadians(getFlapAngle())));
  aerofoilControl.mExtraAngle = asinf(mFlapFraction * FastSin(DegreesToRadians(getFlapAngle())));
  aerofoilControl.mCDScale = mAeroplane->GetAeroplaneSettings().mDragScale;

  mLastWash = Vector3(0,0,0);
  mLastForce = Vector3(0,0,0);
  for (Aerofoils::iterator it = mAerofoils.begin() ; it != mAerofoils.end() ; ++it)
  {
    Aerofoil* aerofoil = *it;
    Vector3 wash, force;
    aerofoil->UpdatePrePhysics(deltaTime, aerofoilControl, turbulenceData, wash, force);
    mLastWash += wash / (float) mAerofoils.size();
    mLastForce += force;
    mAeroplane->IncrementDebugAerofoilIndex();
  }
}

//---------------------------------------------------------------------------------------------------------------------
void Wing::UpdatePostPhysics(float deltaTime)
{
  for (Aerofoils::iterator it = mAerofoils.begin() ; it != mAerofoils.end() ; ++it)
  {
    Aerofoil* aerofoil = *it;
    aerofoil->UpdatePostPhysics(deltaTime);
  }
}

