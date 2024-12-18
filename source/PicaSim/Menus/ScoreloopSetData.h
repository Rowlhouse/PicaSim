#ifndef SCORELOOPSETDATA_H
#define SCORELOOPSETDATA_H

#include <string>

#include "../Scoreloop.h"

enum ScoreloopSetType
{
  SCORELOOPSETTYPE_USERNAME,
  SCORELOOPSETTYPE_EMAIL
};

void ScoreloopSetData(const struct GameSettings& gameSettings, ScoreloopSetType scoreloopSetType);

#endif
