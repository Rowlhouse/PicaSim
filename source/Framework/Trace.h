#ifndef TRACE_H
#define TRACE_H

//#define COMPILE_WITHOUT_TRACE

/*
Macros/functions to support logging and tracing. We must be able to:

1. compile it all out

2. have it compiled in, but disable it at the cost of only one test
per trace line

3. Enable it on a per file basis

4. Enable it/disable at different levels - guidelines are:

0 for infrequent but significant trace (given that the overall trace
and file trace is on) - e.g. object creation/deletion

1 for infrequent but interesting trace

2 for infrequent trace that is very verbose - e.g. dumping out lots of data

3 for significant frequent trace - e.g. key functions that get
called once per frame.

4 for frequent trace that is more verbose.

Enabling levels 0-3 shouldn't have a big impact on
performance. Higher levels will have a significant hit on the
application.

The tracing macro looks like this:

TRACE_IF(int level, char * trace_string)

or

TRACE_FILE_IF(int level)

which calls the first macro with __FILE__ as the trace_string
argument.

This resolve into a simple (or complex) "if", which is followed by a
print-style macro to do the actual tracing. Multiple lines of
tracing can be done with a single test by using braces - e.g.

TRACE_FILE_IF(3) TRACE("value = %d", m_value);

or

TRACE_IF(3, "file.cpp") 
{
int a = calc_value(m_value);
TRACE("val1 = %d, val2 = %d", m_value, a);
}

TRACE may at some point write the output to file etc. 
*/

#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

/////////////////////////////////////////////////////////////////
// These are the interface
/////////////////////////////////////////////////////////////////

/// enable/disable overall trace
inline void EnableTrace(bool enable);

/// set the overall trace level
inline void SetTraceLevel(int level);

/// trace all strings?
inline void EnableTraceAllStrings(bool enable);

/// add a string to the list of traced strings
inline void AddTraceStrings(std::vector<std::string> traceStrings); 
inline void AddTraceString(const std::string & traceString); 
inline void AddTraceString(const char * traceString);

enum tTraceLevels
{
    ONCE_1 = 1, // v. significant, once-off trace
    ONCE_2 = 2,
    ONCE_3 = 3, // low significance

    FRAME_1 = 4, // v. significant once-per-frame trace
    FRAME_2 = 5,
    FRAME_3 = 6, // low significance

    MULTI_FRAME_1 = 7, // v. significant multiple-times-per-frame trace
    MULTI_FRAME_2 = 8,
    MULTI_FRAME_3 = 9
};

/// for now just printf (in the future may make a copy to file)
// go away MS
#ifdef TRACE
#undef TRACE
#endif
#define TRACE TracePrintf

#define USE_FUNCTION

#ifdef USE_FUNCTION
#define TRACE_FUNCTION() \
    TRACE("%s():%d: ", __FUNCTION__, __LINE__)
#else
#define TRACE_FUNCTION() \
    TRACE("%s:%d: ", __FILE__, __LINE__)
#endif

#ifdef USE_FUNCTION
#define TRACE_METHOD() \
    TRACE("%s:%s():%d [%p]: ", __FILE__, __FUNCTION__, __LINE__, this)
#else
#define TRACE_METHOD() \
    TRACE("%s:%d: [%p]: ", __FILE__, __LINE__, this)
#endif

#ifdef USE_FUNCTION
#define TRACE_METHOD_STATIC() \
    TRACE("%s:%s():%d [static]: ", __FILE__, __FUNCTION__, __LINE__)
#else
#define TRACE_METHOD_STATIC() \
    TRACE("%s:%d: [static]: ", __FILE__, __LINE__)
#endif

#ifdef COMPILE_WITHOUT_TRACE
//=======================================================

#define TRACE_IF(level, traceString) if (0)

#define TRACE_FILE_IF(level) if (0)

// just display the function
#define TRACE_FUNCTION_ONLY(level) {}

#define TRACE_METHOD_ONLY(level) {}

#define TRACE_METHOD_STATIC_ONLY(level) {}

//=======================================================
#else
//=======================================================
// with trace
#define TRACE_IF(level, traceString) \
    if ( (traceEnabled) && \
    ((level) <= traceLevel) && \
    ( (traceAllStrings == true) || \
    (CheckTraceString(traceString)) ) )

#define TRACE_FILE_IF(level) \
    TRACE_IF(level, __FILE__)

// just display the function
#define TRACE_FUNCTION_ONLY(level) \
    TRACE_FILE_IF(level) {TRACE_FUNCTION();}

#define TRACE_METHOD_ONLY(level) \
    TRACE_FILE_IF(level) {TRACE_METHOD();}

#define TRACE_METHOD_STATIC_ONLY(level) \
    TRACE_FILE_IF(level) {TRACE_METHOD_STATIC();}

//=======================================================
#endif

/////////////////////////////////////////////////////////////////
// don't look below here
/////////////////////////////////////////////////////////////////

/// Overall trace enabled
extern bool traceEnabled;

/// The overall trace level - only trace with a level equal to or less
/// than this comes out.
extern int traceLevel;

/// The strings for which trace is enabled. Normally these will be
/// file names, though they don't have to be. 
extern std::vector<std::string> traceStrings;

/// If this flag is set, all trace strings are enabled
extern bool traceAllStrings;

/// do the output
void TracePrintf(const char *format, ...);

/// enable/disable overall trace
inline void EnableTrace(bool enable) {traceEnabled = enable;}

inline void EnableTraceAllStrings(bool enable) {
    traceAllStrings = enable;}

inline void SetTraceLevel(int level) {traceLevel = level;}

inline void AddTraceString(const std::string &traceString)
{
    traceStrings.push_back(traceString); 
    std::sort(traceStrings.begin(), traceStrings.end());
}

inline void AddTraceString(const char * traceString)
{
    AddTraceString(std::string(traceString));
}

inline void AddTraceStrings(std::vector<std::string> newTraceStrings)
{
    for (unsigned i = 0 ; i < newTraceStrings.size() ; ++i)
    {
        traceStrings.push_back(newTraceStrings[i]); 
        std::sort(traceStrings.begin(), traceStrings.end());
    }
}

inline bool CheckTraceString(const char * traceString)
{
    return (std::binary_search(traceStrings.begin(), 
        traceStrings.end(),
        std::string(traceString)));
}


#endif
