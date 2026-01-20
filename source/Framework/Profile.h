#ifndef PROFILE_H
#define PROFILE_H

// Profiling system - was Marmalade IwProfile, now stubbed out
// Can be replaced with a custom profiling system later if needed

// Stub macros for profiling (no-op in release)
#define PROFILE_BEGIN(name) ((void)0)
#define PROFILE_END() ((void)0)

// Marmalade-style profiling macros (no-op)
#define IW_PROFILE(name) ((void)0)
#define IW_PROFILE_START(name) ((void)0)
#define IW_PROFILE_STOP() ((void)0)

// IW_USE_PROFILE is not defined, so the debug menu code is disabled

#ifdef IW_USE_PROFILE

// Legacy Marmalade profiling code would go here
// Currently disabled as we don't have Marmalade

inline void createDebugMenu() {}
inline void destroyDebugMenu() {}

#endif

#endif
