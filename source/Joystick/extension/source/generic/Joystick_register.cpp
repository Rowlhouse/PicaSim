/*
 * WARNING: this is an autogenerated file and will be overwritten by
 * the extension interface script.
 */
/*
 * This file contains the automatically generated loader-side
 * functions that form part of the extension.
 *
 * This file is awlays compiled into all loaders but compiles
 * to nothing if this extension is not enabled in the loader
 * at build time.
 */
#include "Joystick_autodefs.h"
#include "s3eEdk.h"
#include "Joystick.h"
//Declarations of Init and Term functions
extern s3eResult JoystickInit();
extern void JoystickTerminate();


void JoystickRegisterExt()
{
    /* fill in the function pointer struct for this extension */
    void* funcPtrs[2];
    funcPtrs[0] = (void*)GetJoystickStatus;
    funcPtrs[1] = (void*)CalibrateJoystick;

    /*
     * Flags that specify the extension's use of locking and stackswitching
     */
    int flags[2] = { 0 };

    /*
     * Register the extension
     */
    s3eEdkRegister("Joystick", funcPtrs, sizeof(funcPtrs), flags, JoystickInit, JoystickTerminate, 0);
}

#if !defined S3E_BUILD_S3ELOADER

#if defined S3E_EDK_USE_STATIC_INIT_ARRAY
int JoystickStaticInit()
{
    void** p = g_StaticInitArray;
    void* end = p + g_StaticArrayLen;
    while (*p) p++;
    if (p < end)
        *p = (void*)&JoystickRegisterExt;
    return 0;
}

int g_JoystickVal = JoystickStaticInit();

#elif defined S3E_EDK_USE_DLLS
S3E_EXTERN_C S3E_DLL_EXPORT void RegisterExt()
{
    JoystickRegisterExt();
}
#endif

#endif