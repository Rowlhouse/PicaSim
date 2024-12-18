/*
* windows-specific implementation of the gamepad extension.
* Add any platform-specific functionality here.
*/
/*
* NOTE: This file was originally written by the extension builder, but will not
* be overwritten (unless --force is specified) and is intended to be modified.
*/
#include "gamepad_internal.h"
#include <s3eEdk_windows.h>
#include "IwDebug.h"
#include <windows.h>
#include <mmsystem.h>
#include <regstr.h>

static UINT gamepad_device_count = 0;
static UINT gamepad_device_handlers[16];
static JOYINFOEX gamepad_device_info[16];
static JOYINFOEX gamepad_device_info_old[16];
static JOYCAPS gamepad_device_caps[16];
typedef char device_name[256];
static device_name gamepad_device_name[16];
static HHOOK hook = 0;

static int gamepadGetOEMProductName ( int index )
{
  char buffer [ 256 ];

  char OEMKey [ 256 ];

  sprintf_s(gamepad_device_name[index],"Microsoft PC joystick driver");

  HKEY  hKey;
  DWORD dwcb;
  LONG  lr;

  /* Open .. MediaResources\CurrentJoystickSettings */
  sprintf_s( buffer, "%s\\%s\\%s", REGSTR_PATH_JOYCONFIG, &gamepad_device_caps[index].szRegKey, REGSTR_KEY_JOYCURR );

  lr = RegOpenKeyEx ( HKEY_CURRENT_USER, buffer, 0, KEY_QUERY_VALUE, &hKey);

  if ( lr != ERROR_SUCCESS ) {
    IwTrace(GAMEPAD_VERBOSE, ("Can't open registry %s for JOYSTICKID%d", buffer, index));
    return 0;
  }

  /* Get OEM Key name */
  dwcb = sizeof(OEMKey);

  /* JOYSTICKID1-16 is zero-based; registry entries for VJOYD are 1-based. */
  sprintf_s ( buffer, "Joystick%d%s", index + 1, REGSTR_VAL_JOYOEMNAME );

  lr = RegQueryValueEx ( hKey, buffer, 0, 0, (LPBYTE) OEMKey, &dwcb);
  RegCloseKey ( hKey );

  if ( lr != ERROR_SUCCESS ) return 0;

  /* Open OEM Key from ...MediaProperties */
  sprintf_s ( buffer, "%s\\%s", REGSTR_PATH_JOYOEM, OEMKey );

  lr = RegOpenKeyEx ( HKEY_LOCAL_MACHINE, buffer, 0, KEY_QUERY_VALUE, &hKey );

  if ( lr != ERROR_SUCCESS ) return 0;

  /* Get OEM Name */
  dwcb = 255;

  lr = RegQueryValueEx ( hKey, REGSTR_VAL_JOYOEMNAME, 0, 0, (LPBYTE) gamepad_device_name[index], &dwcb );
  RegCloseKey ( hKey );

  if ( lr != ERROR_SUCCESS ) return 0;

  return 1;
}

void gamepadReleaseDevices()
{
  for (uint32 i=0; i<gamepad_device_count && i<15; ++i)
  {
    MMRESULT mmResult = joyReleaseCapture(gamepad_device_handlers[i]);
    if (JOYERR_NOERROR == mmResult)
    {
      IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d released", gamepad_device_handlers[i]-JOYSTICKID1+1));
    }
    else
    {
      switch (mmResult)
      {
      case MMSYSERR_NODRIVER:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be released: MMSYSERR_NODRIVER", i+1));
        break;
      default:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be released: 0x%08X", i+1, mmResult));
        break;
      }
    }
  }
  gamepad_device_count = 0;

}
void gamepadCaptureDevices()
{
  gamepad_device_count = 0;
  for (uint32 i=0; i<joyGetNumDevs() && i<15; ++i)
  {
    MMRESULT mmResult = joySetCapture(s3eEdkGetHwnd(), JOYSTICKID1+i, 10, FALSE);
    if (JOYERR_NOERROR == mmResult)
    {
      gamepad_device_handlers[gamepad_device_count] = JOYSTICKID1+i;
      IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d captured", i+1));
      joyGetDevCaps(JOYSTICKID1+i, &gamepad_device_caps[gamepad_device_count], sizeof(JOYCAPS));
      IwTrace(GAMEPAD_VERBOSE, ("JOYSTICKID%d szPname=%s", JOYSTICKID1+i, gamepad_device_caps[gamepad_device_count].szPname));
      IwTrace(GAMEPAD_VERBOSE, ("JOYSTICKID%d szRegKey=%s", JOYSTICKID1+i, gamepad_device_caps[gamepad_device_count].szRegKey));
      IwTrace(GAMEPAD_VERBOSE, ("JOYSTICKID%d szOEMVxD=%s", JOYSTICKID1+i, gamepad_device_caps[gamepad_device_count].szOEMVxD));
      gamepadGetOEMProductName(gamepad_device_count);
      ++gamepad_device_count;
    }
    else
    {
      switch (mmResult)
      {
      case MMSYSERR_NODRIVER:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: MMSYSERR_NODRIVER", i+1));
        break;
      case MMSYSERR_INVALPARAM:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: MMSYSERR_INVALPARAM", i+1));
        break;
      case JOYERR_NOCANDO:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: JOYERR_NOCANDO", i+1));
        break;
      case JOYERR_UNPLUGGED:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: JOYERR_UNPLUGGED", i+1));
        break;
      case JOYERR_PARMS:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: JOYERR_PARMS", i+1));
        break;
      default:
        IwTrace(GAMEPAD_VERBOSE, ("joystick JOYSTICKID%d failed to be captured: 0x%08X", i+1, mmResult));
        break;
      }
    }
  }
}

void gamepadUpdate_platform()
{
  for (uint32 i=0; i<gamepad_device_count;++i)
  {
    uint32 j = gamepad_device_handlers[i];
    LPJOYINFOEX pInfo = &gamepad_device_info[i];
    LPJOYINFOEX pOldInfo = &gamepad_device_info_old[i];
    pInfo->dwSize = sizeof(JOYINFOEX);
    pInfo->dwFlags = JOY_RETURNALL;
    joyGetPosEx(j, pInfo);

    *pOldInfo = *pInfo;
  }
}

LRESULT CALLBACK gamepadGetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
  MSG* msg = (MSG*)lParam;

  switch (msg->message)
  {
  case MM_JOY1MOVE :                     // changed position 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY1MOVE (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  case MM_JOY2MOVE :                     // changed position 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY2MOVE (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  case MM_JOY1BUTTONDOWN :               // button is down 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY1BUTTONDOWN (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  case MM_JOY1BUTTONUP :                 // button is up 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY1BUTTONUP (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  case MM_JOY2BUTTONDOWN :               // button is down 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY2BUTTONDOWN (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  case MM_JOY2BUTTONUP :                 // button is up 
    {
      auto fwButtons = wParam;
      auto xPos = LOWORD(lParam);
      auto yPos = HIWORD(lParam);
      IwTrace(GAMEPAD_VERBOSE, ("MM_JOY2BUTTONUP (%08X, %d, %d)", fwButtons, xPos, yPos));
    }
    break; 
  }

  return CallNextHookEx(hook, code, wParam, lParam); 
}

void gamepadCalibrate_platform()
{
  IwTrace(GAMEPAD, ("Launching calibration"));
  LPSTR lpCommandLine = "control.exe joy.cpl";

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  CreateProcess(
    NULL,
    lpCommandLine,
    NULL, NULL,
    FALSE, 0, NULL, NULL,
    &si,
    &pi);
}

void gamepadReset_platform()
{
  gamepadReleaseDevices();
  gamepadCaptureDevices();
}
s3eResult gamepadInit_platform()
{
  IwTrace(GAMEPAD_VERBOSE, ("gamepadInit"));

  gamepadCaptureDevices();
  gamepadUpdate_platform();

  DWORD ThreadId = GetWindowThreadProcessId(s3eEdkGetHwnd(), NULL);

  hook = SetWindowsHookEx( WH_GETMESSAGE, (HOOKPROC)gamepadGetMsgProc, GetModuleHandle(NULL), ThreadId );
  if (!hook)
  {
    IwTrace(GAMEPAD_VERBOSE, ("Hook is not set"));
    gamepadTerminate_platform();
    return S3E_RESULT_ERROR;
  }
  IwTrace(GAMEPAD_VERBOSE, ("Hook is set"));
  return S3E_RESULT_SUCCESS;
}

void gamepadTerminate_platform()
{
  IwTrace(GAMEPAD_VERBOSE, ("gamepadTerminate"));

  gamepadReleaseDevices();

  if (hook != 0)
  {
    UnhookWindowsHookEx(hook);
    IwTrace(GAMEPAD_VERBOSE, ("Hook is unset"));
    hook = 0;
  }
}

uint32 gamepadGetNumDevices_platform()
{
  return gamepad_device_count;
}

uint32 gamepadGetNumAxes_platform(uint32 index)
{
  return gamepad_device_caps[index].wMaxAxes;
}

uint32 gamepadGetNumButtons_platform(uint32 index)
{
  return gamepad_device_caps[index].wMaxButtons;
}

uint32 gamepadIsPointOfViewAvailable_platform(uint32 index)
{
  return (gamepad_device_caps[index].wCaps & JOYCAPS_HASPOV);
}
int32 gamepadGetPointOfViewAngle_platform(uint32 index)
{
  int32 pov = gamepad_device_info[index].dwPOV;
  if (pov == 65535)
    return -1;
  return (pov * 4096 / 36000);
}

uint32 gamepadGetButtons_platform(uint32 index)
{
  return (gamepad_device_info[index].dwButtons);
}

int32 gamepadGetAxis_platform(uint32 index, uint32 axisIndex)
{
  JOYINFOEX* info = &gamepad_device_info[index];
  JOYCAPS* caps = &gamepad_device_caps[index];
  switch (axisIndex)
  {
  case 0:
    return (info->dwXpos-caps->wXmin)*8192/(caps->wXmax - caps->wXmin)-4096;
  case 1:
    return (info->dwYpos-caps->wYmin)*8192/(caps->wYmax - caps->wYmin)-4096;
  case 2:
    return (info->dwZpos-caps->wZmin)*8192/(caps->wZmax - caps->wZmin)-4096;
  case 3:
    return (info->dwRpos-caps->wRmin)*8192/(caps->wRmax - caps->wRmin)-4096;
  case 4:
    return (info->dwUpos-caps->wUmin)*8192/(caps->wUmax - caps->wUmin)-4096;
  case 5:
    return (info->dwVpos-caps->wVmin)*8192/(caps->wVmax - caps->wVmin)-4096;
  default:
    return 0;
  }
}

uint32 gamepadGetDeviceId_platform(uint32 index)
{
  return gamepad_device_handlers[index];
}
