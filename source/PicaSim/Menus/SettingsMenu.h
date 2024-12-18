#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <string>

/// Displays the settings menu
void DisplaySettingsMenu(struct GameSettings& gameSettings, struct SettingsChangeActions& actions);

enum SelectResult
{
  SELECTRESULT_SELECTED,
  SELECTRESULT_CANCELLED
};
struct IncludeCallback;

SelectResult SelectAeroplane(std::string& file, GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText, IncludeCallback* includeCallback);
SelectResult SelectAndLoadAeroplane(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText);
SelectResult SelectAndLoadEnvironment(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText);

enum ScenarioResult
{
  SCENARIO_TRAINERGLIDER,
  SCENARIO_TRAINERPOWERED,
  SCENARIO_CHOOSE,
  SCENARIO_DEFAULT,
  SCENARIO_CANCELLED
};

ScenarioResult SelectScenario(GameSettings& gameSettings, const char* title, const char* cancelButtonText, const char* altButtonText);

#endif
