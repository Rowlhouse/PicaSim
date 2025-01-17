#ifndef FRAMEWORKSETTINGS_H
#define FRAMEWORKSETTINGS_H

//#include <s3eDevice.h>

#include "Entete.h"
#include <SDL2/SDL_video.h>

struct FrameworkSettings
{
  FrameworkSettings();

  void UpdateScreenDimensions() const;

  //void UpdateOS();

  //bool isWin10() const { return mOS == "Windows"; }
  //bool isWindowsDesktop() const { return mOS == "Windows"; }
  //bool isWindows() const { return mOS == "Windows"; }
  //bool isAndroid() const { return mOS == "Android"; }
  //bool isIOS() const { return mOS == "iOS"; }

  int mPhysicsSubsteps;
  float mNearClipPlaneDistance;
  float mFarClipPlaneDistance;
  bool  mUseMultiLights;

  // Remplacer s3eDeviceOSID par un type std::string pour l'OS
  OS mOS;

  mutable int mScreenWidth, mScreenHeight;

  // Pour stocker les informations sur l'affichage via SDL
  mutable SDL_DisplayMode mDisplayMode;
};



/* struct FrameworkSettings
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
}; */
#endif
