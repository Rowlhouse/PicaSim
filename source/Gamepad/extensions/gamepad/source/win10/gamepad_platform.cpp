/*
 * win10-specific implementation of the gamepad extension.
 * Add any platform-specific functionality here.
 */
 /*
  * NOTE: This file was originally written by the extension builder, but will not
  * be overwritten (unless --force is specified) and is intended to be modified.
  */
#include <crtdbg.h>

#define _CRT_PERROR_DEFINED
#include <wrl/client.h>
#include <wrl/module.h>
#undef _CRT_PERROR_DEFINED

#include "gamepad_internal.h"

#include "gamepad_shim.h"
#include "IwDebug.h"

// This only works with the xbox controller (or generic gamepads?).
// See https://msdn.microsoft.com/en-us/library/dd254801.aspx
// Can other controllers be made to look like the xbox controller?
// http://www.x360ce.com/
// The problem is that the exectuable (E:\code\PicaSim\source\Gamepad\examples\gamepad\build_gamepadexample_vc14_win10devtoolset\bin\x86\)
// doesn't load xinput - it loads Windows.Gaming.Input.dll
// Note that "Use of legacy DirectInput is not recommended, and DirectInput is not available for Windows Store apps."
// https://msdn.microsoft.com/en-us/library/windows/desktop/hh405051(v=vs.85).aspx
// XInput api:
// https://msdn.microsoft.com/en-gb/library/windows/desktop/microsoft.directx_sdk.reference.xinputenable(v=vs.85).aspx

#include "XInput.h"

#pragma comment(lib, "Xinput.lib")

using namespace gamepadExtension;

void gamepadManagedRegister_platform()
{
  gamepadShim^ shim = ref new gamepadShim();
  gamepadNative^ native = ref new gamepadNative();
  s3e_native::s3eExtensionsShim::GetInterface()->Register(ref new gamepadManaged::gamepadManaged(native, shim));

};

s3eResult gamepadInit_platform()
{
  gamepadManagedRegister_platform();

  if (gamepadShim::GetInterface() == nullptr)
  {
    IwTrace(GAMEPAD, ("No IManagedS3EgamepadAPI interface. Managed part of gamepad extension failed to register."));
    return S3E_RESULT_ERROR;
  }

  // Add any platform-specific initialisation code here
  return S3E_RESULT_SUCCESS;
}

void gamepadTerminate_platform()
{
}

#define MAX_NUM_DEVICES 4

struct GamePadState
{
  bool                mAvailable;
  XINPUT_STATE        mInputState;
  XINPUT_CAPABILITIES mInputCapabilities;
};

static GamePadState sGamePadStates[MAX_NUM_DEVICES];

uint32 gamepadGetNumDevices_platform()
{
  uint32 numDevices = 0;
  for (DWORD i = 0; i != MAX_NUM_DEVICES; ++i)
  {
    if (sGamePadStates[i].mAvailable)
      ++numDevices;
  }
  return numDevices;
}

uint32 gamepadGetDeviceId_platform(uint32 index)
{
  return index;
}

uint32 gamepadGetNumAxes_platform(uint32 index)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;
  return 6;
}

uint32 gamepadGetNumButtons_platform(uint32 index)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;
  return 32;
}

uint32 gamepadGetButtons_platform(uint32 index)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;

  return sGamePadStates[index].mInputState.Gamepad.wButtons;
}

inline int32 floatToReturn(float f)
{
  int32 val = (int32) (f * 4096.0f);
  if (val > 4096)
    return 4096;
  else if (val < -4096)
    return -4096;
  else
    return val;
}

int32 gamepadGetAxis_platform(uint32 index, uint32 axisIndex)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;

  int32 val = 0;
  float thumbMax = 32767.0f;
  switch (axisIndex)
  {
  case 0:
    val = floatToReturn(sGamePadStates[index].mInputState.Gamepad.sThumbLX / thumbMax); break;
  case 1:
    val = floatToReturn(sGamePadStates[index].mInputState.Gamepad.sThumbLY / thumbMax); break;
  case 2:
    val = floatToReturn(sGamePadStates[index].mInputState.Gamepad.sThumbRX / thumbMax); break;
  case 3:
    val = floatToReturn(sGamePadStates[index].mInputState.Gamepad.sThumbRY / thumbMax); break;
  case 4:
    val = (sGamePadStates[index].mInputState.Gamepad.bLeftTrigger * 32) - 4095; break;
  case 5:
    val = (sGamePadStates[index].mInputState.Gamepad.bRightTrigger * 32) - 4095; break;
  }
  return val;
}

uint32 gamepadIsPointOfViewAvailable_platform(uint32 index)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;
  return 0;
}

int32 gamepadGetPointOfViewAngle_platform(uint32 index)
{
  if (index >= MAX_NUM_DEVICES || !sGamePadStates[index].mAvailable)
    return 0;
  return 0;
}

void gamepadUpdate_platform()
{
  IwTrace(GAMEPAD_VERBOSE, ("gamepadUpdate"));

  for (DWORD i = 0; i != MAX_NUM_DEVICES; ++i)
  {
    XInputGetCapabilities(i, 0, &sGamePadStates[i].mInputCapabilities);
    DWORD result = XInputGetState(i, &sGamePadStates[i].mInputState);
    if (result == ERROR_SUCCESS)
    {
      sGamePadStates[i].mAvailable = true;
    }
    else
    {
      sGamePadStates[i].mAvailable = false;
    }
  }
}

void gamepadReset_platform()
{
  IwTrace(GAMEPAD, ("gamepadReset"));
}

void gamepadCalibrate_platform()
{
  IwTrace(GAMEPAD, ("gamepadCalibrate"));
}
