#ifndef MEMORYCHECK_H
#define MEMORYCHECK_H

#define USE_PARANOID_MEMTEST 0

void InitMemoryOverrunCheck();

#if USE_PARANOID_MEMTEST
#define MEMTEST()   IwMemBucketGet()->DebugTestIntegrity()
#else
#define MEMTEST()
#endif

#endif

