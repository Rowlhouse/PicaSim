#include "FrameworkSettings.h"
#include "Trace.h"
#include "Helpers.h"

#include <s3e.h>
#include <IwDebug.h>

#include <cstring>
#include <algorithm>

//---------------------------------------------------------------------------------------------------------------------
FrameworkSettings::FrameworkSettings() 
  : 
  mPhysicsSubsteps(8),
  mNearClipPlaneDistance(0.5f),
  mFarClipPlaneDistance(50000.0f),
  mUseMultiLights(true)
{
  mOS = (s3eDeviceOSID) s3eDeviceGetInt(S3E_DEVICE_OS);
  //mOS = S3E_OS_ID_ANDROID;
  TRACE_FILE_IF(1) TRACE("OS = %d", mOS);

  UpdateScreenDimensions();
}

//---------------------------------------------------------------------------------------------------------------------
void FrameworkSettings::UpdateScreenDimensions() const
{
  // Alternatively
  mScreenWidth = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
  mScreenHeight = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

  if (IsMarmaladeVersionLessThan(6,2,0) && mOS == S3E_OS_ID_IPHONE)
    std::swap(mScreenWidth, mScreenHeight);

  // MARMALDE WORKAROUND - screen dimensions are flipped on the first update. Assume landscape.
  if (mOS == S3E_OS_ID_IPHONE && mScreenHeight > mScreenWidth)
  {
    std::swap(mScreenWidth, mScreenHeight);
  }
}