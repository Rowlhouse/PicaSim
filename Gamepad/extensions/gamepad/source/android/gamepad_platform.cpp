/*
 * android-specific implementation of the gamepad extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "gamepad_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"
#include "gamepad.h"

static jobject g_Obj;
static jmethodID g_gamepadGetNumDevices;
static jmethodID g_gamepadGetDeviceId;
static jmethodID g_gamepadGetNumAxes;
static jmethodID g_gamepadGetNumButtons;
static jmethodID g_gamepadGetButtons;
static jmethodID g_gamepadGetAxis;
static jmethodID g_gamepadUpdate;
static jmethodID g_gamepadInit;
static jmethodID g_gamepadTerminate;

s3eResult gamepadInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("gamepad");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    // Get all the extension methods
    g_gamepadGetNumDevices = env->GetMethodID(cls, "gamepadGetNumDevices", "()I");
    if (!g_gamepadGetNumDevices)
        goto fail;

    g_gamepadGetDeviceId = env->GetMethodID(cls, "gamepadGetDeviceId", "(I)I");
    if (!g_gamepadGetDeviceId)
        goto fail;

    g_gamepadGetNumAxes = env->GetMethodID(cls, "gamepadGetNumAxes", "(I)I");
    if (!g_gamepadGetNumAxes)
        goto fail;

    g_gamepadGetNumButtons = env->GetMethodID(cls, "gamepadGetNumButtons", "(I)I");
    if (!g_gamepadGetNumButtons)
        goto fail;

    g_gamepadGetButtons = env->GetMethodID(cls, "gamepadGetButtons", "(I)I");
    if (!g_gamepadGetButtons)
        goto fail;

    g_gamepadGetAxis = env->GetMethodID(cls, "gamepadGetAxis", "(II)F");
    if (!g_gamepadGetAxis)
        goto fail;

    g_gamepadUpdate = env->GetMethodID(cls, "gamepadUpdate", "()V");
    if (!g_gamepadUpdate)
        goto fail;

    g_gamepadInit = env->GetMethodID(cls, "gamepadInit", "()V");
    g_gamepadTerminate = env->GetMethodID(cls, "gamepadTerminate", "()V");

    IwTrace(GAMEPAD, ("GAMEPAD init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    env->CallIntMethod(g_Obj, g_gamepadInit);

    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(gamepad, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void gamepadTerminate_platform()
{
    // Add any platform-specific termination code here
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallIntMethod(g_Obj, g_gamepadTerminate);
}

void gamepadReset_platform()
{
	JNIEnv* env = s3eEdkJNIGetEnv();
	env->CallIntMethod(g_Obj, g_gamepadTerminate);
	env->CallIntMethod(g_Obj, g_gamepadInit);
}

void gamepadCalibrate_platform()
{
}

uint32 gamepadGetNumDevices_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (uint32)env->CallIntMethod(g_Obj, g_gamepadGetNumDevices);
}

uint32 gamepadGetDeviceId_platform(uint32 index)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (uint32)env->CallIntMethod(g_Obj, g_gamepadGetDeviceId, index);
}

uint32 gamepadGetNumAxes_platform(uint32 index)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (uint32)env->CallIntMethod(g_Obj, g_gamepadGetNumAxes, index);
}

uint32 gamepadGetNumButtons_platform(uint32 index)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (uint32)env->CallIntMethod(g_Obj, g_gamepadGetNumButtons, index);
}

uint32 gamepadGetButtons_platform(uint32 index)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (uint32)env->CallIntMethod(g_Obj, g_gamepadGetButtons, index);
}

int32 gamepadGetAxis_platform(uint32 index, uint32 axisIndex)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (int32)(4096*(float)env->CallFloatMethod(g_Obj, g_gamepadGetAxis, index, axisIndex));
}

uint32 gamepadIsPointOfViewAvailable_platform(uint32 index)
{
	return 0;
}

int32 gamepadGetPointOfViewAngle_platform(uint32 index)
{
	return -1;
}

void gamepadUpdate_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    env->CallVoidMethod(g_Obj, g_gamepadUpdate);
}
