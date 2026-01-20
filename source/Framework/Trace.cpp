// JigLib - Copyright (C) 2004 Danny Chapman
//
// \file trace.cpp

#include "Trace.h"

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
        logFile = fopen("trace.txt", "w");

        if (logFile == NULL)
        {
            fprintf(stderr, "Unable to open trace.txt\n");
            // We have a backup plan!! Assume that non-win32 is unix based.
#ifdef _WIN32
            fprintf(stderr, "Trying C:\\trace.txt\n");
            char logFileName[] = "C:\\trace.txt";
#else
            fprintf(stderr, "Trying /tmp/trace.txt\n");
            char logFileName[] = "/tmp/trace.txt";
#endif
            logFile = fopen(logFileName, "w");
            if (logFile == NULL)
            {
                fprintf(stderr, "Unable to open backup logfile %s\n", logFileName);
            }
            else
            {
                printf("Opened log file: %s\n", logFileName);
            }
        }
        else
        {
            printf("Opened log file: trace.txt\n");
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
