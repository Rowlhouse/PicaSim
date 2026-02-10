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
    mUseMultiLights(true),
    mLogLevel(1)
{
    // Get platform type
    mPlatform = Platform::GetPlatformID();
    TRACE_FILE_IF(ONCE_1) TRACE("Platform = %s", Platform::GetPlatformName());

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
        mScreenWidth = Platform::GetDisplayWidth();
        mScreenHeight = Platform::GetDisplayHeight();
    }

    // On iOS, ensure landscape orientation (width > height)
    if (mPlatform == Platform::PlatformID::iOS && mScreenHeight > mScreenWidth)
    {
        std::swap(mScreenWidth, mScreenHeight);
    }
}
