#ifndef FRAMEWORKSETTINGS_H
#define FRAMEWORKSETTINGS_H

#include "../Platform/Platform.h"
#include "../Platform/S3ECompat.h"

struct FrameworkSettings
{
    FrameworkSettings();

    // Obviously not really const, but sometimes this just needs to be updated before things can work with it
    void UpdateScreenDimensions() const;

    bool isWin10() const { return mOS == S3E_OS_ID_WIN10; }
    bool isWindowsDesktop() const { return mOS == S3E_OS_ID_WINDOWS; }
    bool isWindows() const { return mOS == S3E_OS_ID_WINDOWS || mOS == S3E_OS_ID_WIN10; }
    bool isAndroid() const { return mOS == S3E_OS_ID_ANDROID; }
    bool isIOS() const { return mOS == S3E_OS_ID_IPHONE; }

    int mPhysicsSubsteps;
    float mNearClipPlaneDistance;
    float mFarClipPlaneDistance;
    bool  mUseMultiLights;

    s3eDeviceOSID mOS;
    mutable int mScreenWidth, mScreenHeight;
};

#endif
