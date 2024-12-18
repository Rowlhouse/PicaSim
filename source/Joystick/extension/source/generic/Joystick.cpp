/*
Generic implementation of the Joystick extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "Joystick_internal.h"
s3eResult JoystickInit()
{
    //Add any generic initialisation code here
    return JoystickInit_platform();
}

void JoystickTerminate()
{
    //Add any generic termination code here
    JoystickTerminate_platform();
}

s3eResult GetJoystickStatus(JoystickData& joystick, int id)
{
	return GetJoystickStatus_platform(joystick, id);
}

void CalibrateJoystick()
{
  return CalibrateJoystick_platform();
}