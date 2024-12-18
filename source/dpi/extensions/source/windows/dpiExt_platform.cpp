/*
 * windows-specific implementation of the dpiExt extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "dpiExt_internal.h"
#include <Windows.h>

s3eResult dpiExtInit_platform()
{
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
}

void dpiExtTerminate_platform()
{
    // Add any platform-specific termination code here
}

int dpiExtGetDeviceDPI_platform()
{
	HDC hScreenDC = GetDC( NULL );
	int PixelsX = GetDeviceCaps( hScreenDC, HORZRES );
	int MMX = GetDeviceCaps( hScreenDC, HORZSIZE );
	ReleaseDC( NULL, hScreenDC );	
    return 254*PixelsX/MMX/10;
}
