#ifndef SCORELOOPMENU_H
#define SCORELOOPMENU_H

#include <s3eTypes.h>

struct Score
{
  Score(double result, double minorResult, uint32 mode) : mResult(result), mMinorResult(minorResult), mMode(mode) {}
  double mResult;
  double mMinorResult;
  uint32 mMode;
};

/// Displays the Scoreloop system, initially in the profile tab
void DisplayScoreloopMenu(struct GameSettings& gameSettings);

/// Displays the scoreloop system, initially in the leaderboard in game settings, and using the score as the status
void DisplayScoreloopMenuLeaderboard(struct GameSettings& gameSettings, const Score* score);

/// Uploads the score, and displays the menu on the leaderboard tab
void UploadScoreloop(struct GameSettings& gameSettings, const Score& score);


#endif
