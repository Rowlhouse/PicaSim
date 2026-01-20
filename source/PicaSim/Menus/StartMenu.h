#ifndef STARTMENU_H
#define STARTMENU_H

#include "../GameSettings.h"

enum StartMenuResult
{
    STARTMENU_FLY,
    STARTMENU_CHALLENGE,
    STARTMENU_QUIT,
    STARTMENU_SETTINGS,
    STARTMENU_HELP,
    STARTMENU_REFRESH,
    STARTMENU_MAX
};

/// Displays the start menu, and indicates the next action. Also may change the configuration/setttings
StartMenuResult DisplayStartMenu(struct GameSettings& gameSettings);


#endif
