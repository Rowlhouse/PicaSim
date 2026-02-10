// JigLib - Copyright (C) 2004 Danny Chapman
//
// \file trace.cpp

#include "Trace.h"
#include "Platform.h"

#include <stdio.h>

#ifdef PICASIM_ANDROID
#include <android/log.h>
#define ANDROID_LOG_TAG "PicaSim"
#endif

/// Overall trace enabled
bool traceEnabled = true;

/// The overall trace level - only trace with a level equal to or less
/// than this comes out.
int traceLevel = 1;

/// The strings for which trace is enabled. Normally these will be
/// file names, though they don't have to be.
std::vector<std::string> traceStrings;

/// If this flag is set, all trace strings are enabled
bool traceAllStrings = true;

// File-scope statics (promoted from TracePrintf for reset support)
static bool traceFileInit = false;
static FILE* traceLogFile = nullptr;

void ResetTrace()
{
    if (traceLogFile)
    {
        fclose(traceLogFile);
        traceLogFile = nullptr;
    }
    traceFileInit = false;
}

void TracePrintf(const char *fmt, ...)
{
    va_list ap;

    if (traceFileInit == false)
    {
        traceFileInit = true;

        // Use platform-specific writable logs directory
        std::string logsPath = Platform::GetLogsPath();
        FileSystem::MakeDirectory(logsPath);
        std::string logFilePath = logsPath + "trace.txt";

        traceLogFile = fopen(logFilePath.c_str(), "w");

        if (traceLogFile == NULL)
        {
            fprintf(stderr, "Unable to open %s\n", logFilePath.c_str());
        }
        else
        {
            printf("Opened log file: %s\n", logFilePath.c_str());
        }
    }

    // first to stdout
    static char str[2048];
    va_start(ap, fmt);
    vsnprintf(str, 2047, fmt, ap);
    str[2047] = 0;
    va_end(ap);

    // Output to stdout (replaces Marmalade IwTrace)
#ifdef PICASIM_ANDROID
    __android_log_print(ANDROID_LOG_INFO, ANDROID_LOG_TAG, "%s", str);
#else
    printf("%s\n", str);
#endif

    // now to file
    if (traceLogFile)
    {
        fprintf(traceLogFile, "%s\n", str);
        // flush it line-by-line so we don't miss any
        fflush(traceLogFile);
    }
}
