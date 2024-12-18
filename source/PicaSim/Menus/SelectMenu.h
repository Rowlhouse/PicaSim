#ifndef SELECTMENU_H
#define SELECTMENU_H

/// Displays the menu based on the strings and returns the index of the item selected (or -1 for cancel)
int SelectItemMenu(const struct GameSettings& gameSettings, const char* items[], int numItems, const char* titleText);

#endif
