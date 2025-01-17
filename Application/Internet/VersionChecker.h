#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

void InitVersionChecker();
void TerminateVersionChecker();

/// Prompts a check if it hasn't been made yet, and returns true if there's a new version available
bool IsNewVersionAvailable();

int GetBuildNumber();

#endif

