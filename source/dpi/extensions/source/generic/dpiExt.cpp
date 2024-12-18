/*
Generic implementation of the dpiExt extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "dpiExt_internal.h"
s3eResult dpiExtInit()
{
    //Add any generic initialisation code here
    return dpiExtInit_platform();
}

void dpiExtTerminate()
{
    //Add any generic termination code here
    dpiExtTerminate_platform();
}

int dpiExtGetDeviceDPI()
{
	return dpiExtGetDeviceDPI_platform();
}
