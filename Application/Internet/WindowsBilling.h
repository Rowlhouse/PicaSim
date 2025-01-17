#ifndef WINDOWSBILLING_H
#define WINDOWSBILLING_H

#include "GameSettings.h"

void InitWindowsBilling(GameSettings& gameSettings);
void TerminateWindowsBilling();

/// Starts an asynchronous check for the trial version
void WindowsCheckForTrialVersion();

/// Starts an asynchronous purchase attempt trial version
void WindowsBuyFullVersion();

#endif

