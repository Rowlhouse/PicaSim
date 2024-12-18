/*
 * Internal header for the Joystick extension.
 *
 * This file should be used for any common function definitions etc that need to
 * be shared between the platform-dependent and platform-indepdendent parts of
 * this extension.
 */

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef JOYSTICK_INTERNAL_H
#define JOYSTICK_INTERNAL_H

#include "s3eTypes.h"
#include "Joystick.h"
#include "Joystick_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult JoystickInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult JoystickInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void JoystickTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void JoystickTerminate_platform();
s3eResult GetJoystickStatus_platform(JoystickData& joystick, int id);
void CalibrateJoystick_platform();

#endif /* !JOYSTICK_INTERNAL_H */