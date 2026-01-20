#include "FrameworkSettings.h"
#include "Trace.h"
#include "Helpers.h"
#include "../Platform/Window.h"

#include <cstring>
#include <algorithm>

// External window reference
extern Window* gWindow;

//======================================================================================================================
FrameworkSettings::FrameworkSettings()
    :
    mPhysicsSubsteps(8),
    mNearClipPlaneDistance(0.5f),
    mFarClipPlaneDistance(50000.0f),
    mUseMultiLights(true)
{
    // Get OS type from Platform abstraction
    mOS = (s3eDeviceOSID)s3eDeviceGetInt(S3E_DEVICE_OS);
    TRACE_FILE_IF(1) TRACE("OS = %d", mOS);

    UpdateScreenDimensions();
}

//======================================================================================================================
void FrameworkSettings::UpdateScreenDimensions() const
{
    // Get screen dimensions from Window if available, otherwise from Platform
    if (gWindow && gWindow->IsInitialized())
    {
        mScreenWidth = gWindow->GetWidth();
        mScreenHeight = gWindow->GetHeight();
    }
    else
    {
        mScreenWidth = Platform::GetScreenWidth();
        mScreenHeight = Platform::GetScreenHeight();
    }

    // On iOS, ensure landscape orientation (width > height)
    if (mOS == S3E_OS_ID_IPHONE && mScreenHeight > mScreenWidth)
    {
        std::swap(mScreenWidth, mScreenHeight);
    }
}
