/*
Generic implementation of the gamepad extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "File.h"
#include "gamepad_internal.h"
s3eResult gamepadInit()
{
    //Add any generic initialisation code here
    return gamepadInit_platform();
}

void gamepadTerminate()
{
    //Add any generic termination code here
    gamepadTerminate_platform();
}

uint32_t gamepadGetNumDevices()
{
	return gamepadGetNumDevices_platform();
}

uint32_t gamepadGetDeviceId(uint32_t index)
{
	return gamepadGetDeviceId_platform(index);
}

uint32_t gamepadGetNumAxes(uint32_t index)
{
	return gamepadGetNumAxes_platform(index);
}

uint32_t gamepadGetNumButtons(uint32_t index)
{
	return gamepadGetNumButtons_platform(index);
}

uint32_t gamepadGetButtons(uint32_t index)
{
	return gamepadGetButtons_platform(index);
}

int32_t gamepadGetAxis(uint32_t index, uint32_t axisIndex)
{
	return gamepadGetAxis_platform(index, axisIndex);
}

uint32_t gamepadIsPointOfViewAvailable(uint32_t index)
{
	return gamepadIsPointOfViewAvailable_platform(index);
}

int32_t gamepadGetPointOfViewAngle(uint32_t index)
{
	return gamepadGetPointOfViewAngle_platform(index);
}

void gamepadUpdate()
{
	gamepadUpdate_platform();
}

void gamepadReset()
{
	gamepadReset_platform();
}

void gamepadCalibrate()
{
	gamepadCalibrate_platform();
}
