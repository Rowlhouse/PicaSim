#ifndef FONCTIONSPOINTERKEYBOARDMARMELADE_H
#define FONCTIONSPOINTERKEYBOARDMARMELADE_H

#include "Entete.h"
// #include "FonctionsMarmelade.h"
// #include <SDL2/SDL_events.h>
// #include <SDL2/SDL_keycode.h>
// #include <SDL2/SDL_mouse.h>
// #include <SDL2/SDL_stdinc.h>
// #include <iostream>



int s3ePointerGetInt (s3ePointerState param);

int s3ePointerGetX();

int s3ePointerGetY();

void s3ePointerUpdate();

void s3eKeyboardUpdate();

bool s3ePointerGetState(s3ePointerState etat);

s3ePointerState s3ePointerGetTouchState(int touchIndex);

float s3ePointerGetTouchX(int touchIndex);

float s3ePointerGetTouchY(int touchIndex);

int s3eKeyboardGetState(SDL_Keycode key);

#endif