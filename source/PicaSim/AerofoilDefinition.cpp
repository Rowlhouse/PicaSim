#include "AerofoilDefinition.h"

#include "AerofoilParameters.h"
#include "Engine.h"
#include "Wing.h"
#include "Aeroplane.h"
#include "AeroplanePhysics.h"
#include "Environment.h"
#include "PicaSim.h"
#include "AIControllerTug.h"

#include "Framework.h"

AerofoilDefinition::AerofoilDefinitions AerofoilDefinition::mAerofoilDefinitions;

const float kinematicViscosity = 1.5e-5f;

//----------------------------------------------------------------------------------------------------------------------
AerofoilDefinition* AerofoilDefinition::Create(const char* name)
{
  AerofoilDefinitions::iterator it = mAerofoilDefinitions.find(name);
  if (it != mAerofoilDefinitions.end())
  {
    ++it->second->mReferenceCount;
    return it->second;
  }

  AerofoilDefinition* aerofoilDefinition = new AerofoilDefinition(name);
  mAerofoilDefinitions[name] = aerofoilDefinition;

  return aerofoilDefinition;
}

//----------------------------------------------------------------------------------------------------------------------
void AerofoilDefinition::Release(AerofoilDefinition* aerofoilDefinition)
{
  AerofoilDefinitions::iterator it = mAerofoilDefinitions.find(aerofoilDefinition->mName);
  IwAssert(ROWLHOUSE, it != mAerofoilDefinitions.end());

  IwAssert(ROWLHOUSE, aerofoilDefinition->mReferenceCount > 0);
  if (--aerofoilDefinition->mReferenceCount == 0)
  {
    delete aerofoilDefinition;
    mAerofoilDefinitions.erase(it);
  }
}

//----------------------------------------------------------------------------------------------------------------------
struct DataPoint
{
  DataPoint() {}
  DataPoint(float angle, float CL, float flying, float turbulentDrag) 
    : mAngle(angle), mCL(CL), mFlying(flying), mTurbulentDrag(turbulentDrag) {}
  bool operator<(const DataPoint& rhs) const {return mAngle < rhs.mAngle;}
  float mAngle, mCL, mFlying, mTurbulentDrag;
};
typedef std::vector<DataPoint> DataPoints;

//----------------------------------------------------------------------------------------------------------------------
AerodynamicData GetAerodynamicData(float angleOfAttack, const DataPoints& dataPoints)
{
  if (angleOfAttack > PI)
    angleOfAttack -= 2.0f * PI;
  else if (angleOfAttack < -PI)
    angleOfAttack += 2.0f * PI;

  size_t numData = dataPoints.size();
  for (size_t i = 0 ; i != numData ; ++i)
  {
    const DataPoint& d0 = dataPoints[i];
    if (d0.mAngle <= angleOfAttack)
    {
      const DataPoint& d1 = dataPoints[(i+1) % numData];
      float angle1 = d1.mAngle;
      if (i == numData-1)
        angle1 += PI * 2.0f;
      if (angle1 >= angleOfAttack)
      {
        float frac = (angleOfAttack - d0.mAngle) / (angle1 - d0.mAngle);

        AerodynamicData data;
        data.mCL = frac * d1.mCL + (1.0f - frac) * d0.mCL;
        data.mFlying = frac * d1.mFlying + (1.0f - frac) * d0.mFlying;
        data.mTurbulentDrag = ClampToRange(frac * d1.mTurbulentDrag + (1.0f - frac) * d0.mTurbulentDrag, 0.0f, 1.0f);
        return data;
      }
    }
  }
  // TODO implement this better and handle out of range etc
  IwAssert(ROWLHOUSE, false);
  AerodynamicData data;
  data.mCL = data.mFlying = data.mTurbulentDrag = 0.0f;
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
static void AddStallRegion(
  int numPts, DataPoints& dataPoints, float angleStart, float angleRange, 
  float CL0, float CLPerDeg, float dragBucketFactor)
{
  for (int i = 0 ; i != numPts ; ++i)
  {
    float frac = i / (numPts - 1.0f);
    float angle = angleStart + frac * angleRange;
    float CL = CL0 + CLPerDeg * angle;
    float flying = 0.5f + 0.5f * FastCos(frac * PI);
    float dragFactor = 1.0f - flying;
    dragFactor = dragBucketFactor + (1.0f - dragBucketFactor) * dragFactor;
    dataPoints.push_back(DataPoint(DegreesToRadians(angle), CL, flying, dragFactor));
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void AddMidRange(
  int numPts, DataPoints& dataPoints, float angleStart, float angleEnd,float CL0, float CLPerDeg, 
  float dragBucketFactor, float dragBucketLowerAngle, float dragBucketUpperAngle, float dragBucketTransitionDistance)
{
  float midAngle = 0.5f * (angleStart + angleEnd);
  float midBucketAngle = 0.5f * (dragBucketLowerAngle + dragBucketUpperAngle);
  float bucketHalfWidth = 0.5f * (dragBucketUpperAngle - dragBucketLowerAngle);
  for (int i = 1 ; i < numPts-1 ; ++i)
  {
    float frac = i / (numPts - 1.0f);
    float angle = angleStart + frac * (angleEnd - angleStart);
    float CL = CL0 + CLPerDeg * angle;

    float a = fabsf(angle - midBucketAngle);
    float t = (a - bucketHalfWidth) / dragBucketTransitionDistance;
    float s = SmoothStep(t);
    float dragFactor = s * dragBucketFactor;

    dataPoints.push_back(DataPoint(DegreesToRadians(angle), CL, 1.0f, dragFactor));
  }
}

//----------------------------------------------------------------------------------------------------------------------
AerofoilDefinition::AerofoilDefinition(const char* name)
  : mReferenceCount(1), mName(name)
{
  char filename[256];
  sprintf(filename, "%s.xml", name);
  TiXmlDocument aerofoilDoc(filename);
  bool ok = aerofoilDoc.LoadFile();
  IwAssert(ROWLHOUSE, ok);

  TiXmlHandle docHandle( &aerofoilDoc );
  TiXmlElement* element = docHandle.FirstChild( "Aerofoil" ).ToElement();
  IwAssert(ROWLHOUSE, element);

  readFromXML(element, "liftPositionOffsetFractionForwards", mLiftPositionOffsetFractionForwards);
  readFromXML(element, "liftPositionOffsetFractionReverse", mLiftPositionOffsetFractionReverse);
  readFromXML(element, "CDFlying", mCDFlying);
  readFromXML(element, "CDStalled", mCDStalled);
  readFromXML(element, "CM0", mCM0);
  readFromXML(element, "CMPerDeg", mCMPerDeg);

  readFromXML(element, "refRe",     mRefRe);
  readFromXML(element, "CDPower",   mCDPower);
  readFromXML(element, "minReFrac", mMinReFrac);

  bool useData = true;
  readFromXML(element, "useData", useData);

  DataPoints dataPoints;
  dataPoints.reserve(64);

  if (useData)
  {
    // Read in the graph data etc
    for (int iData = 0 ; ; ++iData)
    {
      TiXmlElement* dataElement = docHandle.Child("Data", iData).ToElement();
      if (!dataElement)
        break;
    
      DataPoint data;
      data.mAngle = DegreesToRadians(readFloatFromXML(dataElement, "angle")); 
      data.mCL = readFloatFromXML(dataElement, "CL");
      data.mFlying = readFloatFromXML(dataElement, "flying");
      data.mTurbulentDrag = 1.0f - fabsf(data.mFlying);
      readFromXML(dataElement, "turbulentDrag", data.mTurbulentDrag);
      dataPoints.push_back(data);
    }
  }
  else
  {
    float CL0 = 0.0f;
    float CLPerDeg = 0.1f;
    float positiveAttachedAngle = 9.0f;
    float positiveStallRange = 10.0f;
    float negativeAttachedAngle = -9.0f;
    float negativeStallRange = -10.0f;
    float dragBucketFactor = 0.2f;
    float dragBucketWidth = 5.0f;

    readFromXML(element, "CL0",    CL0);
    readFromXML(element, "CLPerDeg",    CLPerDeg);
    readFromXML(element, "positiveAttachedAngle",  positiveAttachedAngle);
    readFromXML(element, "positiveStallRange", positiveStallRange);
    readFromXML(element, "negativeAttachedAngle",  negativeAttachedAngle);
    readFromXML(element, "negativeStallRange", negativeStallRange);

    float dragBucketLowerAngle = negativeAttachedAngle;
    float dragBucketUpperAngle = positiveAttachedAngle;
    readFromXML(element, "dragBucketLowerAngle", dragBucketLowerAngle);
    readFromXML(element, "dragBucketUpperAngle", dragBucketUpperAngle);
    readFromXML(element, "dragBucketFactor", dragBucketFactor);

    float positiveStallCL = CL0 + CLPerDeg * positiveAttachedAngle;
    float negativeStallCL = CL0 + CLPerDeg * negativeAttachedAngle;

    int numPts = 32;

    // The reversed range
    float CL0Rev = -CL0 * 0.25f;
    dataPoints.push_back(DataPoint(DegreesToRadians(160.0f),  -0.1f,         0.0f, 1.0f));
    dataPoints.push_back(DataPoint(DegreesToRadians(170.0f),   CL0Rev-1.0f, -1.0f, 1.0f));
    dataPoints.push_back(DataPoint(DegreesToRadians(-180.0f),  CL0Rev,      -1.0f, 1.0f));
    dataPoints.push_back(DataPoint(DegreesToRadians(-170.0f),  CL0Rev+1.0f, -1.0f, 1.0f));
    dataPoints.push_back(DataPoint(DegreesToRadians(-160.0f),  0.1f,         0.0f, 1.0f));

    // The normal range
    float midAngle = 0.5f * (negativeAttachedAngle + positiveAttachedAngle);
    float dragBucketTransitionDistance = 0.2f * (dragBucketUpperAngle - dragBucketLowerAngle);

    dataPoints.push_back(DataPoint(DegreesToRadians(-90.0f),   0.0f,         0.0f, 1.0f));
    AddStallRegion(numPts, dataPoints, negativeAttachedAngle, negativeStallRange, CL0, CLPerDeg, dragBucketFactor);
    AddMidRange(numPts, dataPoints, negativeAttachedAngle, positiveAttachedAngle, CL0, CLPerDeg, dragBucketFactor, 
      dragBucketLowerAngle, dragBucketUpperAngle, dragBucketTransitionDistance);
    AddStallRegion(numPts, dataPoints, positiveAttachedAngle, positiveStallRange, CL0, CLPerDeg, dragBucketFactor);
    dataPoints.push_back(DataPoint(DegreesToRadians(90.0f),   0.0f,      0.0f, 1.0f));
  }

  std::sort(dataPoints.begin(), dataPoints.end());

  // Interpolate
  const size_t numGraphPoints = 512;
  mGraphData.resize(numGraphPoints);
  for (size_t i = 0 ; i != numGraphPoints ; ++i)
  {
    float angle = (i * 2.0f * PI) / numGraphPoints;
    mGraphData[i] = GetAerodynamicData(angle, dataPoints);
  }

}

//----------------------------------------------------------------------------------------------------------------------
AerofoilDefinition::~AerofoilDefinition()
{
  IwAssert(ROWLHOUSE, mReferenceCount == 0);
}

//----------------------------------------------------------------------------------------------------------------------
void AerofoilDefinition::GetGraphData(float& angleOfAttack, float& CL, float& flying, float& turbulentDrag, float effectiveAR) const
{
  // a0 / (1 + a0 / (pi * AR * eff) )
  const float CLSlope = 0.1f * 180.0f / PI;
  float CLScale = 1.0f / (1.0f + CLSlope / (PI * effectiveAR) );
  float angleOfAttackScale = ClampToRange(CLScale, 0.3f, 1.0f);
  CLScale = CLScale / angleOfAttackScale;

  float effectiveAoA = angleOfAttack;
  const float THREE_HALVES_PI = HALF_PI * 3.0f;
  // Get AoA into -PI/2,3*PI/2 range
  if (effectiveAoA < -HALF_PI)
    effectiveAoA += TWO_PI;
  else if (effectiveAoA > THREE_HALVES_PI)
    effectiveAoA -= TWO_PI;

  if (effectiveAoA < HALF_PI)
    effectiveAoA *= angleOfAttackScale;
  else
    effectiveAoA = PI + (effectiveAoA - PI) * angleOfAttackScale;

  effectiveAoA += 8.0f * PI;
  const size_t numPts = mGraphData.size();
  const float deltaAngle = (2.0f * PI) / numPts;
  const size_t i = (size_t) (effectiveAoA / deltaAngle);
  const float fi = (effectiveAoA - i * deltaAngle) / deltaAngle;
  IwAssert(ROWLHOUSE, fi >= -0.1f);
  IwAssert(ROWLHOUSE, fi <= 1.1f);
  const size_t i0 = i % numPts;
  const size_t i1 = (i0 + 1) % numPts;

  const AerodynamicData& d0 = mGraphData[i0];
  const AerodynamicData& d1 = mGraphData[i1];
  CL = fi * d1.mCL + (1.0f - fi) * d0.mCL;
  flying = fi * d1.mFlying + (1.0f - fi) * d0.mFlying;
  turbulentDrag = fi * d1.mTurbulentDrag + (1.0f - fi) * d0.mTurbulentDrag;

  CL *= CLScale;

  angleOfAttack = (i0 + fi) * deltaAngle;
  if (angleOfAttack > PI)
    angleOfAttack -= PI * 2.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void AerofoilDefinition::GetFlightData(
  const AerofoilConfiguration& configuration,
  const AerofoilControl& control,
  const float effectiveAR,
  float speedSq,
  float angleOfAttack, 
  float& CL, float& CD, float& CM, float& flying) const
{
  IwAssert(ROWLHOUSE, speedSq == speedSq);
  IwAssert(ROWLHOUSE, angleOfAttack == angleOfAttack);

  float controlFrac = configuration.mControlHalfSpeed > 0.0f ? 1.0f / (1.0f + speedSq/Square(configuration.mControlHalfSpeed)) : 1.0f;

  angleOfAttack += control.mExtraAngle * controlFrac;
  const float sinAngleOfAttack = FastSin(angleOfAttack);
  const float sinTwiceAngleOfAttack = FastSin(2.0f * angleOfAttack);

  float turbulentDrag = 0.0f;
  GetGraphData(angleOfAttack, CL, flying, turbulentDrag, effectiveAR);
  float f = fabsf(flying);
  //f = cubicEaseInOut(fabsf(flying), 0.0f, 1.0f, 1.0f);

  // Calculate CL
  float CLFortyFive = 1.1f;
  CLFortyFive *= 1.0f / (1.0f + 1.0f/effectiveAR);

  CL = f * CL + (1.0f - f) * CLFortyFive * sinTwiceAngleOfAttack;

  float effectivenessInStall = 0.3f;
  float effectiveness = 1.0f - (1.0f - effectivenessInStall) * (1.0f - f);

  CL += control.mExtraCL * controlFrac * effectiveness;

  // Calculate CD
  // See http://en.wikipedia.org/wiki/Induced_drag so the expected induced fraction is 
  // CD_induced = CL^2 / (pi * e * AR)
  // where e is the wing span efficiency value by which the induced drag exceeds that of an elliptical lift distribution, typically 0.85 to 0.95,
  // AR is the aspect ratio (length/chord)

  // Scale drag according to Re^pow - see http://www.ewp.rpi.edu/hartford/~ferraj7/ET/Other/References/Laitone1997.pdf
  float speed = sqrtf(speedSq);
  float re = speed * configuration.mExtents.x / kinematicViscosity;
  float reFrac = re / mRefRe;
  reFrac = Maximum(reFrac, mMinReFrac);
  float reScale = powf(reFrac, mCDPower);
  float CDScale = control.mCDScale * reScale;

  // AR correction: http://www.ecn.nl/docs/library/report/2001/rx01004.pdf
  float AR = effectiveAR > 1.0f ? effectiveAR : 1.0f / effectiveAR;
  float CDStalled = 2.0f - 0.82f * (1.0f - expf(-17.0f / AR));
  // Adjust stalled CD for camber
  float extraCDFromCamber = 1.0f * control.mExtraCamber / DegreesToRadians(90.0f);
  CDStalled += angleOfAttack > 0.0f ? extraCDFromCamber : -extraCDFromCamber;
  float minStalledCD = 0.1f;
  float stalledCD = minStalledCD + (CDStalled - minStalledCD) * fabsf(sinAngleOfAttack);
  
  float CDInduced = f * CDScale * CL * CL / (PI * effectiveAR * configuration.mWingSpanEfficiency);
  float CDControl = f * CDScale * control.mExtraCD;
  float CDForm = (1.0f-turbulentDrag) * mCDFlying * CDScale + turbulentDrag * stalledCD;
  CD = CDForm + CDInduced + CDControl;

  CM = mCM0;
  CM += control.mExtraCM;
  if (angleOfAttack < 0.5f * PI && angleOfAttack > -0.5f * PI)
    CM += mCMPerDeg * RadiansToDegrees(angleOfAttack);
  CM *= flying;

  // Adjust CL for the aspect ratio - effectively adjust the CL per alpha, when flying
  float CLAdjusted = CL / (1.0f + fabsf(CL) / (PI * effectiveAR) );
  CL += f * (CLAdjusted -CL);

}

//----------------------------------------------------------------------------------------------------------------------
void AerofoilDefinition::DrawAerofoilPlot(
  const AerofoilConfiguration& configuration, 
  const AerofoilParameters& parameters,
  const AerofoilControl& control,
  float angleOfAttack,
  float speedSq) const
{
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.5f, 0.0f), Vector2(0.5f,1.0f), Vector3(1.0f,1.0f,1.0f));
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.5f), Vector2(1.0f,0.5f), Vector3(1.0f,1.0f,1.0f));
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.25f, 0.0f), Vector2(0.25f,1.0f), Vector3(0,0,0));
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.75f, 0.0f), Vector2(0.75f,1.0f), Vector3(0,0,0));
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.25f), Vector2(1.0f,0.25f), Vector3(0,0,0));
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.75f), Vector2(1.0f,0.75f), Vector3(0,0,0));

    const float CLMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotCLMax;
    const float CDMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotCDMax;
    const float CMMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotCMMax;
    const float LDMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotLDMax;
    const float xRange = DegreesToRadians(PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotAngleRange);

    float x0 = -xRange;
    float CLy0 = 0.5f;
    float CDy0 = 0.5f;
    float LDy0 = 1.0f;
    float CMy0 = 0.5f;
    float f0 = 0.0f;
    size_t numPts = PicaSim::GetInstance().GetSettings().mOptions.mFrameworkSettings.mScreenWidth/8;
    float deltaAngle = 2.0f * xRange / numPts;
    for (size_t iAngle = 0 ; iAngle != numPts ; ++iAngle)
    {
      float angle = -xRange + deltaAngle * iAngle;
      float sinAngleOfAttack = FastSin(angle);
      float sinTwiceAngleOfAttack = FastSin(2.0f * angle);

      float CL, CD, CM, flying;
      GetFlightData(configuration, control, configuration.mWingAspectRatio, speedSq, angle, CL, CD, CM, flying);

      float LD = CD != 0.0f ? CL/CD : 0.0f;

      float x = 0.5f + 0.5f * (angle / xRange);
      float CLy = 0.5f + 0.5f * CL / CLMax;
      float CDy = 0.0f + CD / CDMax;
      float CMy = 0.5f + 0.5f * CM / CMMax;
      float LDy = 0.5f + 0.5f * LD / LDMax;
      float fy = 0.5f + 0.5f * flying;

      if (iAngle != 0)
      {
        RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, CLy0), Vector2(x,CLy), Vector3(0.0f,1.0f,0.0f));
        RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, CDy0), Vector2(x,CDy), Vector3(1.0f,0.0f,0.0f));
        RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, LDy0), Vector2(x,LDy), Vector3(0.0f,0.0f,1.0f));
        RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, CMy0), Vector2(x,CMy), Vector3(0.0f,1.0f,1.0f));
        //RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, f0), Vector2(x,fy), Vector3(0.0f,0.0f,0.0f));
        //RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x, 0.49f), Vector2(x, 0.51f), Vector3(1.0f,0.0f,1.0f));
      }
      x0 = x;
      CLy0 = CLy;
      CDy0 = CDy;
      LDy0 = LDy;
      CMy0 = CMy;
      f0 = fy;
    }

    // Show the current position on the curve
    float x = 0.5f + 0.5f * (angleOfAttack / xRange);
    RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x, 0.0f), Vector2(x, 1.0f), Vector3(1.0f,0.0f,1.0f));

    // Draw the aerofoil position
    RenderManager::GetInstance().GetDebugRenderer().DrawPoint(parameters.mTM, 0.5f, Vector3(1,1,1));

    // Write the current data
    float CL, CD, CM, flying;
    GetFlightData(configuration, control, configuration.mWingAspectRatio, speedSq, angleOfAttack, CL, CD, CM, flying);
    float LD = CD != 0.0f ? CL/CD : 0.0f;
    float re = sqrtf(speedSq) * configuration.mExtents.x / kinematicViscosity;

    char txt[256];
    float startY = 0.25f;
    float deltaY = 0.05f;
    int iY = 0;
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(mName.c_str(), Vector2(0.01f, startY + iY++ * deltaY), Vector3(0,0,0));
    sprintf(txt, "AoA = %5.1f deg", RadiansToDegrees(angleOfAttack));
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(1,1,1));
    sprintf(txt, "CL = %5.3f", CL);
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(0,1,0));
    sprintf(txt, "CD = %5.3f", CD);
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(1,0,0));
    sprintf(txt, "LD = %5.1f", LD);
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(0,0,1));
    sprintf(txt, "CM = %5.3f", CM);
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(0,1,1));
    sprintf(txt, "Re = %5.0f", re);
    RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, startY + iY++ * deltaY), Vector3(1,1,1));
}

//----------------------------------------------------------------------------------------------------------------------
void AerofoilDefinition::DrawAerofoilPolar(
  const AerofoilConfiguration& configuration, 
  const AerofoilParameters& parameters,
  const AerofoilControl& control,
  float angleOfAttack,
  float speedSq) const
{
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.5f, 0.0f),  Vector2(0.5f,1.0f),  Vector3(0,0,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.5f),  Vector2(1.0f,0.5f),  Vector3(1,1,1));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.25f, 0.0f), Vector2(0.25f,1.0f), Vector3(0,0,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.75f, 0.0f), Vector2(0.75f,1.0f), Vector3(0,0,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.25f), Vector2(1.0f,0.25f), Vector3(0,0,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(0.0f, 0.75f), Vector2(1.0f,0.75f), Vector3(0,0,0));

  const float CLMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotCLMax;
  const float CDMax = PicaSim::GetInstance().GetSettings().mOptions.mAerofoilPlotCDMax;

  const float angleRange = DegreesToRadians(90.0f);
  const size_t numPts = 256;
  const float deltaAngle = 2.0f * angleRange / numPts;

  float x0 = 0.5f;
  float y0 = 0.5f;
  for (size_t iAngle = 0 ; iAngle != numPts ; ++iAngle)
  {
    float angle = -angleRange + deltaAngle * iAngle;
    float sinAngleOfAttack = FastSin(angle);
    float sinTwiceAngleOfAttack = FastSin(2.0f * angle);
    float CL, CD, CM, flying;
    GetFlightData(configuration, control, configuration.mWingAspectRatio, speedSq, angle, CL, CD, CM, flying);
    float x = CD / CDMax;
    float y = 0.5f + 0.5f * CL / CLMax;
    if (iAngle != 0)
      RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x0, y0), Vector2(x,y), Vector3(1,0,0));
    x0 = x;
    y0 = y;
  }

  // Draw the aerofoil position
  RenderManager::GetInstance().GetDebugRenderer().DrawPoint(parameters.mTM, 0.5f, Vector3(1,1,1));

  // Write the current data
  float CL, CD, CM, flying;
  GetFlightData(configuration, control, configuration.mWingAspectRatio, speedSq, angleOfAttack, CL, CD, CM, flying);
  float LD = CD != 0.0f ? CL/CD : 0.0f;
  const float kinViscosity = 1.42e-5f; // Air at 1atm and 10 deg C
  float re = sqrtf(speedSq) * configuration.mExtents.x / kinViscosity;

  // Show the current position on the curve
  float x = CD / CDMax;
  float y = 0.5f + 0.5f * CL / CLMax;
  float d = 0.02f;
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x-d, y), Vector2(x+d,y), Vector3(0,1,0));
  RenderManager::GetInstance().GetDebugRenderer().DrawLine2D(Vector2(x, y-d), Vector2(x,y+d), Vector3(0,1,0));

  char txt[256];
  sprintf(txt, "AoA = %5.1f deg", RadiansToDegrees(angleOfAttack));
  RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, 0.25f), Vector3(1,1,1));
  sprintf(txt, "CL = %5.3f", CL);
  RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, 0.3f), Vector3(0,1,0));
  sprintf(txt, "CD = %5.3f", CD);
  RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, 0.35f), Vector3(1,0,0));
  sprintf(txt, "LD = %5.1f", LD);
  RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, 0.4f), Vector3(0,0,1));
  sprintf(txt, "Re = %5.0f", re);
  RenderManager::GetInstance().GetDebugRenderer().DrawText2D(txt, Vector2(0.01f, 0.45f), Vector3(1,1,1));
}

//----------------------------------------------------------------------------------------------------------------------
float AerofoilDefinition::ApplyForces(
  Aeroplane* aeroplane, 
  const AerofoilConfiguration& configuration, 
  const AerofoilParameters& parameters,
  const AerofoilControl& control,
  const Environment& environment,
  const TurbulenceData& turbulenceData,
  Vector3& wash,
  Vector3& force,
  float deltaTime,
  float& lastAoA) const
{
  Vector3 globalWind = environment.GetWindAtPosition(parameters.mTM.GetTrans(), Environment::WIND_TYPE_ALL, &turbulenceData);
  IwAssert(ROWLHOUSE, globalWind == globalWind);

  wash = Vector3(0,0,0);

  // Increase effective aspect ratio when near the ground, as that has the effect of 
  // inhibiting the tip vortices and thus induced drag etc.
  float effectiveAR = configuration.mWingAspectRatio;
  Vector3 pos = parameters.mTM.GetTrans();

  if (configuration.mGroundEffect)
  {
    float height = pos.z - Environment::GetInstance().GetTerrain().GetTerrainHeightFast(pos.x, pos.y, true);
    float span  = configuration.mExtents.x * configuration.mWingAspectRatio;
    float heightRatio = 3.0f * height / span;
    if (heightRatio < 1.0f)
    {
      heightRatio = Maximum(heightRatio, 0.5f);
      effectiveAR *= 1.0f / heightRatio;
    }
  }

  const AIControllerTug* tugController = aeroplane->GetTugController();
  if (tugController)
  {
    const Aeroplane* tugAeroplane = tugController->GetAeroplane();
    if (tugAeroplane)
    {
      const AeroplanePhysics::Engines& engines = tugAeroplane->GetPhysics()->GetEngines();
      for (size_t iEngine = 0 ; iEngine != engines.size() ; ++iEngine)
      {
        const Engine* engine = engines[iEngine];
        Transform engineTM = engine->GetTM();
        Vector3 washAngVel;
        Vector3 engineWash = engine->GetLastWash(washAngVel);
        Vector3 offset = parameters.mTM.GetTrans() - engineTM.GetTrans(); 

        if (engineWash.Dot(offset) > 0.0f)
        {
          float washSpeed = engineWash.GetLength();
          float offsetLength = offset.GetLength();
          if (offsetLength * washSpeed > 0.0f)
          {
            // Airflow in local space
            Vector3 airflow = engineWash + globalWind - parameters.mVel;
            float d = airflow.Dot(offset);
            // Rotate it according to our rotation, to estimate the effect of the lag (which results in a stabilising effect).
            if (d > 0.0f)
            {
              Vector3 offsetDir = offset / offsetLength;
              Vector3 washDir = engineWash / washSpeed;

              float sideOffset = (offset - offset.Dot(washDir) * washDir).GetLength();
              float radius = engine->GetRadius();
              float washRadius = radius * 2.0f;;
              // Sideways drop-off
              float frac = ClampToRange(1.0f - (sideOffset - washRadius) / washRadius, 0.0f, 1.0f);
              engineWash *= frac;
              // Distance drop-off
              frac = Square(radius/washRadius);
              engineWash *= frac;
              globalWind += engineWash;
            }
          }
        }
      }
    }
  }

  if (configuration.mWashFromEngineFraction != 0.0f)
  {
    const Engine* engine = aeroplane->GetPhysics()->GetEngine(configuration.mWashFromEngineName);
    if (engine)
    {
      Transform engineTM = engine->GetTM();
      Vector3 washAngVel;
      Vector3 engineWash = engine->GetLastWash(washAngVel);
      Vector3 offset = parameters.mTM.GetTrans() - engineTM.GetTrans(); 

      if (engineWash.Dot(offset) > 0.0f)
      {
        float washSpeed = engineWash.GetLength();
        float offsetLength = offset.GetLength();
        if (offsetLength * washSpeed > 0.0f)
        {
          // Airflow in local space
          float overallWashFraction = 0.5f * (configuration.mWashFromEngineFraction + 1.0f);
          Vector3 airflow = engineWash * overallWashFraction + globalWind - parameters.mVel;
          float d = airflow.Dot(offset);
          // Rotate it according to our rotation, to estimate the effect of the lag (which results in a stabilising effect).
          if (d > 0.0f)
          {
            Vector3 offsetDir = offset / offsetLength;
            Vector3 washDir = engineWash / washSpeed;

            float lagTime = offset.GetLengthSquared() / d;

            offset += (globalWind - parameters.mVel) * lagTime;
            float sideOffset = (offset - offset.Dot(washDir) * washDir).GetLength();

            float radius = engine->GetRadius();
            radius += offsetLength * 0.5f;

            float frac = ClampToRange(1.0f - (sideOffset - radius) / radius, 0.0f, 1.0f);
            engineWash *= frac;

            const Vector3 angVel = aeroplane->GetAngularVelocity();
            float rotationMagnitude = angVel.GetLength() * lagTime;
            if (rotationMagnitude > 0.0f)
            {
              const Quat q(angVel.GetNormalised(), rotationMagnitude);
              engineWash = q.RotateVector(engineWash);
            }
            globalWind += engineWash * configuration.mWashFromEngineFraction;
          }

          if (washAngVel.GetLengthSquared() > 0.0f)
          {
            Vector3 washDir = washAngVel.GetNormalised();
            Vector3 offset = parameters.mTM.GetTrans() - engineTM.GetTrans();
            offset -= offset.Dot(washDir) * washDir;

            Vector3 sideWind = washAngVel.Cross(offset);
            globalWind += sideWind * configuration.mWashFromEngineFraction;
          }
        }
      }
    }
  }

  for (size_t iWash = 0 ; iWash != configuration.sMaxWashFromWings ; ++iWash)
  {
    if (configuration.mWashFromWingFraction[iWash] != 0.0f)
    {
      const Wing* wing = aeroplane->GetPhysics()->GetWing(configuration.mWashFromWingName[iWash]);
      if (wing)
      {
        Vector3 wingPos = (wing->GetTMLocal() * aeroplane->GetPhysics()->GetTransform()).GetTrans();
        Vector3 airflow = globalWind - parameters.mVel;
        Vector3 offset = parameters.mTM.GetTrans() - wingPos; 

        float d = airflow.Dot(offset);
        Vector3 wash = wing->GetLastWash();

        // Rotate it according to our rotation, to estimate the effect of the lag (which results in a stabilising effect).
        if (d > 0.0f)
        {
          float lagTime = offset.GetLengthSquared() / d;

          const Vector3 angVel = aeroplane->GetAngularVelocity();
          float rotationMagnitude = angVel.GetLength() * lagTime;
          if (rotationMagnitude > 0.0f)
          {
            const Quat q(angVel.GetNormalised(), rotationMagnitude);
            wash = q.RotateVector(wash);
          }
        }

        Vector3 airflowDir = airflow.GetNormalised();
        Vector3 dirFromWing = offset.GetNormalised();
        float dot = dirFromWing.Dot(airflowDir);
        if (dot > 0.0f)
        {
          globalWind += dot * wash * configuration.mWashFromWingFraction[iWash];
        }
      }
    }
  }

  // Airflow is relative to the aerofoil so +ve X is forward, +ve Y is left, +ve Z is up
  Vector3 airflow = parameters.mTM.GetTranspose().RotateVec(globalWind - parameters.mVel);
  Vector3 airflowDir = airflow.GetNormalised();

  float shadowAmount = Minimum(airflowDir.Dot(configuration.mShadow), 1.0f);
  if (shadowAmount > 0.0f)
  {
    airflow *= 1.0f - shadowAmount;
  }
  if (0)
  {
    Vector3 shadowWorld = parameters.mTM.RotateVec(configuration.mShadow);
    RenderManager::GetInstance().GetDebugRenderer().DrawVector(
      pos, shadowWorld * 0.5f, Vector3(0,1,1));
  }
  if (0)
  {
    Vector3 airflowWorld = parameters.mTM.RotateVec(airflow);
    RenderManager::GetInstance().GetDebugRenderer().DrawVector(
      pos, airflowWorld * 0.02f, Vector3(1,1,0));
  }


  float speedSq = Square(airflow.x) + Square(airflow.y) + Square(airflow.z);
  const float minSpeed = 0.000001f;
  if (speedSq < Square(minSpeed))
  {
    speedSq = Square(minSpeed);
    airflow.x = -minSpeed;
  }

  // Adjust AR depending on the direction
  //if (0)
  {
    float beta = atan2f(fabsf(airflow.y), fabsf(airflow.x));
    float span = effectiveAR * configuration.mExtents.x;
    float area = span * configuration.mExtents.x;
    float effSpan = span * FastCos(beta) + configuration.mExtents.x * FastSin(beta);
    effectiveAR = Square(effSpan) / area;
  }

#if 1
  float tangentialAirSpeed = Hypot(airflow.x, airflow.y);
  tangentialAirSpeed = airflow.x > 0.0f ? tangentialAirSpeed : -tangentialAirSpeed;
  float angleOfAttack = atan2f(airflow.z, -tangentialAirSpeed);
#else
  float angleOfAttack = atan2f(airflow.z, -airflow.x);
#endif
  float CL, CD, CM, flying;
  GetFlightData(configuration, control, effectiveAR, speedSq, angleOfAttack, CL, CD, CM, flying);

#if 0
  // Effective camber change caused by pitch rate
  if (deltaTime > 0.0f)
  {
    float deltaAoA = angleOfAttack - lastAoA;
    deltaAoA = WrapToRange(deltaAoA, -PI, PI);
    float alphaRate = deltaAoA / deltaTime;

    float s = alphaRate / (8.0f * airflow.GetLength() * configuration.mExtents.x);
    float deltaCL = s * 12.0f;
    deltaCL = ClampToRange(deltaCL, -0.1f, 0.1f);
    CL += deltaCL;
  }
#endif
  lastAoA = angleOfAttack;

  // If below the plain height apply lots of drag
  if (parameters.mTM.GetTrans().z < PicaSim::GetInstance().GetSettings().mEnvironmentSettings.mTerrainSettings.mPlainHeight)
    CD = 2.0f;

  // Calculate the force application point
  Vector3 forceApplicationPoint = parameters.mTM.GetTrans();
  if (flying > 0.0f)
    forceApplicationPoint += parameters.mTM.RowX() * flying * mLiftPositionOffsetFractionForwards * configuration.mExtents.x * 0.5f;
  else
    forceApplicationPoint -= parameters.mTM.RowX() * flying * mLiftPositionOffsetFractionReverse * configuration.mExtents.x * 0.5f;

  // Calculate the forces
  
  const float airDensity = aeroplane->GetAirDensity();
  float dynamicPressure = 0.5f * airDensity * speedSq;

  // These forces are relative to the airflow (not to the airofoil)
  float liftForce = configuration.mArea * dynamicPressure * CL;
  float dragForce = configuration.mArea * dynamicPressure * CD;

  float pitchingMoment = -CM * configuration.mArea * dynamicPressure * configuration.mExtents.x;

#if 0
  Transform windTM;
  windTM.SetAxisAngle(parameters.mTM.RowY(), angleOfAttack);

  Vector3 dragDir = -windTM.RotateVec(parameters.mTM.RowX());
  Vector3 liftDir = windTM.RotateVec(parameters.mTM.RowZ());
#else
  Transform windTM;
  windTM.SetAxisAngle(parameters.mTM.RowY(), angleOfAttack);

  Vector3 dragDir = parameters.mTM.RotateVec(airflow.GetNormalised());
  Vector3 rightDir = parameters.mTM.RowZ().Cross(dragDir).GetNormalised();

  //RenderManager::GetInstance().GetDebugRenderer().DrawVector(
  //  forceApplicationPoint, rightDir, Vector3(1,1,1));
  //RenderManager::GetInstance().GetDebugRenderer().DrawVector(
  //  forceApplicationPoint, parameters.mTM.RowZ(), Vector3(0,0,0));

  Vector3 liftDir = dragDir.Cross(rightDir).GetNormalised();
  if (airflow.x > 0.0f)
    liftDir = -liftDir;

#endif
  Vector3 pitchingDir = dragDir.Cross(liftDir);

  force = liftForce * liftDir + dragForce * dragDir;
  Vector3 moment = pitchingMoment * pitchingDir;

#if 0
  Vector3 angularVelocity = aeroplane->GetPhysics()->GetAngularVelocity();
  Vector3 localAngularVelocity(parameters.mTM.RowX().Dot(angularVelocity), parameters.mTM.RowY().Dot(angularVelocity), parameters.mTM.RowZ().Dot(angularVelocity));
  float angularCD = 1.0f; // CD for the element that's integrated
  float angularDragConstant = angularCD * airDensity / (4.0f * 8.0f);
  Vector3 localDragMoment = -angularDragConstant * localAngularVelocity;
  localDragMoment.x *= configuration.mExtents.x * Hypercube(configuration.mExtents.y) * fabsf(localAngularVelocity.x);
  localDragMoment.y *= configuration.mExtents.y * Hypercube(configuration.mExtents.x) * fabsf(localAngularVelocity.y);
  localDragMoment.z = 0.0f;
  Vector3 dragMoment = parameters.mTM.RotateVec(localDragMoment);
  moment += dragMoment;
#endif

  aeroplane->GetPhysics()->ApplyWorldForceAtWorldPosition(force, forceApplicationPoint);
  aeroplane->GetPhysics()->ApplyWorldTorque(moment);

  // Downwash
  // http://www.onemetre.net/Design/Downwash/Momentum/Momentum.htm
  float downwashAngle = (2.0f / PI) * CL / effectiveAR;
  wash = (downwashAngle * airflow.x) * liftDir;

  // Debug Draw
  const Options& options = PicaSim::GetInstance().GetSettings().mOptions;
  if (options.mAerofoilInfo == aeroplane->GetDebugAerofoilIndex())
  {
    AerofoilConfiguration config = configuration;
    float spSq = speedSq;
    if (options.mAerofoilPlotReference)
    {
      config.mWingAspectRatio = 1e12f;
      // re = speed * chord / kinematicViscosity
      spSq = Square(mRefRe * kinematicViscosity / configuration.mExtents.x);
    }
    else
    {
      config.mWingAspectRatio = effectiveAR;
    }
    if (options.mAerofoilPlotPolar)
      DrawAerofoilPolar(config, parameters, control, angleOfAttack, spSq);
    else
      DrawAerofoilPlot(config, parameters, control, angleOfAttack, spSq);
  }

  if (options.mStallMarkers && fabsf(flying < 0.99f))
  {
    const Vector3 colour(1.0f - flying, flying, 0.0f);
    float stallMarkerSize = configuration.mExtents.x * 0.3f;
    RenderManager::GetInstance().GetDebugRenderer().DrawPoint(
      forceApplicationPoint, stallMarkerSize, colour);
  }

#if 0
  const float torqueRenderScale = 10.3f;

  RenderManager::GetInstance().GetDebugRenderer().DrawVector(
    forceApplicationPoint, moment * torqueRenderScale, Vector3(1,1,0));
#endif
#if 0
  const float forceRenderScale = 1.0f;

  //RenderManager::GetInstance().GetDebugRenderer().DrawVector(
  //  forceApplicationPoint, dragDir, Vector3(0,0,0));

  //RenderManager::GetInstance().GetDebugRenderer().DrawVector(
  //  forceApplicationPoint, liftDir, Vector3(1,1,1));

  RenderManager::GetInstance().GetDebugRenderer().DrawVector(
    forceApplicationPoint, dragDir * dragForce * forceRenderScale, Vector3(1,0,0));

  RenderManager::GetInstance().GetDebugRenderer().DrawVector(
    forceApplicationPoint, liftDir * liftForce * forceRenderScale, Vector3(0,1,0));

  RenderManager::GetInstance().GetDebugRenderer().DrawVector(
    forceApplicationPoint, force * forceRenderScale, Vector3(0,0,1));
#endif

  return flying;
}

