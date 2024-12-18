/*
 * android-specific implementation of the dpiExt extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "dpiExt_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jobject g_Obj;
static jmethodID g_dpiExtGetDeviceDPI;

s3eResult dpiExtInit_platform()
{
    //Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    //Get the extension class
    jclass cls = s3eEdkAndroidFindClass("dpiExt");
    if (!cls)
        goto fail;

    //Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    //Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    //Get all the extension methods
    g_dpiExtGetDeviceDPI = env->GetMethodID(cls, "dpiExtGetDeviceDPI", "()I");
    if (!g_dpiExtGetDeviceDPI)
        goto fail;



    IwTrace(DPIEXT, ("DPIEXT init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteLocalRef(cls);
    
    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;
    
fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(dpiExt, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void dpiExtTerminate_platform()
{
    // Add any platform-specific termination code here
}

int dpiExtGetDeviceDPI_platform()
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    return (int)env->CallIntMethod(g_Obj, g_dpiExtGetDeviceDPI);
}
