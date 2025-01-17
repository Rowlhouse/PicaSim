/*
 * Internal header for the gamepad extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef GAMEPAD_INTERNAL_H
#define GAMEPAD_INTERNAL_H

//#include "s3eTypes.h"
#include "File.h"
#include "gamepad.h"
#include "gamepad_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult gamepadInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult gamepadInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void gamepadTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void gamepadTerminate_platform();
uint32_t gamepadGetNumDevices_platform();

uint32_t gamepadGetDeviceId_platform(uint32_t index);

uint32_t gamepadGetNumAxes_platform(uint32_t index);

uint32_t gamepadGetNumButtons_platform(uint32_t index);

uint32_t gamepadGetButtons_platform(uint32_t index);

int32_t gamepadGetAxis_platform(uint32_t index, uint32_t axisIndex);

uint32_t gamepadIsPointOfViewAvailable_platform(uint32_t index);

int32_t gamepadGetPointOfViewAngle_platform(uint32_t index);

void gamepadUpdate_platform();

void gamepadReset_platform();

void gamepadCalibrate_platform();


#endif /* !GAMEPAD_INTERNAL_H */