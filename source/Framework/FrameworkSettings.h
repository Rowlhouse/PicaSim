#ifndef FRAMEWORKSETTINGS_H
#define FRAMEWORKSETTINGS_H

#include "../Platform/Platform.h"

struct FrameworkSettings
{
    FrameworkSettings();

    // Obviously not really const, but sometimes this just needs to be updated before things can work with it
    void UpdateScreenDimensions() const;

    bool isWindows() const { return mPlatform == Platform::PlatformID::Windows; }
    bool isAndroid() const { return mPlatform == Platform::PlatformID::Android; }
    bool isIOS() const { return mPlatform == Platform::PlatformID::iOS; }
    bool isMacOS() const { return mPlatform == Platform::PlatformID::macOS; }
    bool isLinux() const { return mPlatform == Platform::PlatformID::Linux; }
    bool isDesktop() const { return Platform::IsDesktop(); }
    bool isMobile() const { return Platform::IsMobile(); }

    int mPhysicsSubsteps;
    float mNearClipPlaneDistance;
    float mFarClipPlaneDistance;
    bool  mUseMultiLights;

    int mLogLevel;

    Platform::PlatformID mPlatform;
    mutable int mScreenWidth, mScreenHeight;
};

#endif
