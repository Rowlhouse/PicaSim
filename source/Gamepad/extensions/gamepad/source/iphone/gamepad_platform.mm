/*
 * iphone-specific implementation of the gamepad extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "gamepad_internal.h"

s3eResult gamepadInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void gamepadTerminate_platform()
{
    // Add any platform-specific termination code here
}

uint32 gamepadGetNumDevices_platform()
{
    return 0;
}

uint32 gamepadGetDeviceId_platform(uint32 index)
{
    return 0;
}

const char* gamepadGetDeviceName_platform(uint32 index)
{
    return 0;
}

uint32 gamepadGetNumAxes_platform(uint32 index)
{
    return 0;
}

uint32 gamepadGetNumButtons_platform(uint32 index)
{
    return 0;
}

uint32 gamepadGetButtons_platform(uint32 index)
{
    return 0;
}

int32 gamepadGetAxis_platform(uint32 index, uint32 axisIndex)
{
    return 0;
}

uint32 gamepadIsPointOfViewAvailable_platform(uint32 index)
{
    return 0;
}

int32 gamepadGetPointOfViewAngle_platform(uint32 index)
{
    return 0;
}

void gamepadRegisterCallback_platform(s3eCallback callback, void* userData)
{
}

void gamepadUnregisterCallback_platform(s3eCallback callback)
{
}

void gamepadUpdate_platform()
{
}

void gamepadReset_platform()
{
}

void gamepadCalibrate_platform()
{
}
