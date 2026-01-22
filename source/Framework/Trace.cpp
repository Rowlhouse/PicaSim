// JigLib - Copyright (C) 2004 Danny Chapman
//
// \file trace.cpp

#include "Trace.h"
#include "Platform.h"

#include <stdio.h>

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

void TracePrintf(const char *fmt, ...)
{
    va_list ap;

    // prepare log file
    static bool init = false;
    static FILE * logFile = 0;

    if (init == false)
    {
        init = true;

        // Use platform-specific writable logs directory
        std::string logsPath = Platform::GetLogsPath();
        FileSystem::MakeDirectory(logsPath);
        std::string logFilePath = logsPath + "trace.txt";

        logFile = fopen(logFilePath.c_str(), "w");

        if (logFile == NULL)
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
    printf("%s\n", str);

    // now to file
    if (logFile)
    {
        fprintf(logFile, "%s\n", str);
        // flush it line-by-line so we don't miss any
        fflush(logFile);
    }
}
