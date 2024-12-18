/*
Internal header for the dpiExt extension.

This file should be used for any common function definitions etc that need to
be shared between the platform-dependent and platform-indepdendent parts of
this extension.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#ifndef DPIEXT_H_INTERNAL
#define DPIEXT_H_INTERNAL

#include "s3eTypes.h"
#include "dpiExt.h"
#include "dpiExt_autodefs.h"


/**
 * Initialise the extension.  This is called once then the extension is first
 * accessed by s3eregister.  If this function returns S3E_RESULT_ERROR the
 * extension will be reported as not-existing on the device.
 */
s3eResult dpiExtInit();

/**
 * Platform-specific initialisation, implemented on each platform
 */
s3eResult dpiExtInit_platform();

/**
 * Terminate the extension.  This is called once on shutdown, but only if the
 * extension was loader and Init() was successful.
 */
void dpiExtTerminate();

/**
 * Platform-specific termination, implemented on each platform
 */
void dpiExtTerminate_platform();
int dpiExtGetDeviceDPI_platform();


#endif /* DPIEXT_H_INTERNAL */