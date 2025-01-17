#include "PicaJoystick.h"
#include <cstring>
#include "File.h"
#include "FonctionsMarmelade.h"
#include "Trace.h"
#include <cstdint>
#include "GameSettings.h"
#include "Entete.h"
#include "gamepad.h"


// #include <IwGx.h>

static JoystickData sJoystickData;

//---------------------------------------------------------------------------------------------------------------------
JoystickData::JoystickData() 
{
  memset(this, 0, sizeof(*this));
}

//---------------------------------------------------------------------------------------------------------------------
s3eBool JoystickAvailable()
{
  if (!gamepadAvailable() || gamepadGetNumDevices() == 0)
    return S3E_FALSE;
  else
    //return true;
    return S3E_TRUE;
}

//---------------------------------------------------------------------------------------------------------------------
s3eResult GetJoystickStatus(JoystickData& joystick, int id)
{
  TRACE_FUNCTION_ONLY(2);

  if (!JoystickAvailable())
    return S3E_RESULT_ERROR;

  joystick = sJoystickData;

  return S3E_RESULT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
void CalibrateJoystick()
{
  gamepadCalibrate();
}

//---------------------------------------------------------------------------------------------------------------------
void UpdateJoystick(int id)
{
  TRACE_FILE_IF(2) TRACE("Updating joystick %d\n", id);

  gamepadUpdate();

  sJoystickData = JoystickData();
  if (JoystickAvailable() && id >= 0 && id < (int) gamepadGetNumDevices())
  {
    TRACE_FILE_IF(2) TRACE("Copying gamepad results");
    // Map [-4096,4096] to [0,65535]
    uint32_t numAxes = gamepadGetNumAxes(id);
    for (uint32_t i = 0 ; i < numAxes && i < JoystickData::MAX_ANALOGUEINPUTS ; ++i)
      sJoystickData.mAnalogueInputs[i] = (long) ((4096 + gamepadGetAxis(id, i)) * 65535.0f/8192.0f);

    if (gamepadIsPointOfViewAvailable(id))
      sJoystickData.mPOVDirections[0] = gamepadGetPointOfViewAngle(id);
 
    uint32_t buttons = gamepadGetButtons(id);
    for (uint32_t i = 0 ; i != JoystickData::MAX_BUTTONS ; ++i)
      sJoystickData.mButtons[i] = (buttons & 1 << i) ? 128 : 0;

#if 0
    char txt[64];
    sprintf(txt, "buttons = %u", buttons);
    IwGxPrintString(10, 10, txt);
#endif

    sprintf(sJoystickData.mName, "Joystick %d", id);
  }
  else
  {
    sprintf(sJoystickData.mName, "Joystick %d unavailable", id);
    TRACE_FILE_IF(2) TRACE("No gamepad available");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void ResetJoystick()
{
  gamepadReset();
}

//---------------------------------------------------------------------------------------------------------------------
bool ShowJoystickInGame(const GameSettings& gameSettings)
{
  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_IPHONE)
    return false;

  // Don't display Joystick info on Android < 3 which is API level 11
  if (gameSettings.mOptions.mFrameworkSettings.mOS == S3E_OS_ID_ANDROID)
  {
    int32_t osVersion = s3eDeviceGetInt(S3E_DEVICE_OS_VERSION);
    int32_t majorVersion = osVersion >> 16;
    if (majorVersion < 11)
      return false;
  }

  return true;
}

