#ifndef FONCTIONSPOINTERKEYBOARDMARMELADE_H
#define FONCTIONSPOINTERKEYBOARDMARMELADE_H

#include "FonctionsMarmelade.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_stdinc.h>
#include <iostream>

enum s3ePointerState {
    S3E_POINTER_AVAILABLE,
    S3E_POINTER_BUTTON_MOUSEWHEELDOWN,
    S3E_POINTER_BUTTON_MOUSEWHEELUP,
    S3E_POINTER_STATE_DOWN,
    S3E_POINTER_STATE_UP,
    S3E_POINTER_STATE_UNKNOWN
};

#define S3E_KEY_STATE_PRESSED SDL_PRESSED
#define S3E_KEY_STATE_DOWN SDL_KEYDOWN
#define S3E_KEY_STATE_UP SDL_KEYUP

enum s3eKey {
    s3eKey1 = SDL_SCANCODE_1,
    s3eKey2 = SDL_SCANCODE_2,
    s3eKey3 = SDL_SCANCODE_3,
    s3eKey4 = SDL_SCANCODE_4,
    s3eKey5 = SDL_SCANCODE_5,
    s3eKey6 = SDL_SCANCODE_6,
    s3eKey7 = SDL_SCANCODE_7,
    s3eKey8 = SDL_SCANCODE_8,
    s3eKey9 = SDL_SCANCODE_9,
    s3eKey0 = SDL_SCANCODE_0,

    // Numpad keys
    s3eKeyNumPad0 = SDL_SCANCODE_KP_0,
    s3eKeyNumPad1 = SDL_SCANCODE_KP_1,
    s3eKeyNumPad2 = SDL_SCANCODE_KP_2,
    s3eKeyNumPad3 = SDL_SCANCODE_KP_3,
    s3eKeyNumPad4 = SDL_SCANCODE_KP_4,
    s3eKeyNumPad5 = SDL_SCANCODE_KP_5,
    s3eKeyNumPad6 = SDL_SCANCODE_KP_6,
    s3eKeyNumPad7 = SDL_SCANCODE_KP_7,
    s3eKeyNumPad8 = SDL_SCANCODE_KP_8,
    s3eKeyNumPad9 = SDL_SCANCODE_KP_9,

    s3eKeyMinus = SDLK_MINUS,
    s3eKeyEquals = SDLK_EQUALS,
    s3eKeyLeft = SDLK_LEFT,
    s3eKeyRight = SDLK_RIGHT,
    s3eKeyRightShift = SDLK_RSHIFT,
    s3eKeyLeftControl = SDLK_LCTRL,
    s3eKeyDown = SDLK_DOWN,
    s3eKeyUp = SDLK_UP,
    s3eKeyBack = SDLK_BACKSLASH,
    s3eKeyEsc = SDLK_ESCAPE,
    s3eKeySearch = SDLK_AC_SEARCH,
    s3eKeyMenu = SDLK_MENU,

    // Alphabet keys
    s3eKeyA = SDLK_a,
    s3eKeyB = SDLK_b,
    s3eKeyC = SDLK_c,
    s3eKeyD = SDLK_d,
    s3eKeyE = SDLK_e,
    s3eKeyF = SDLK_f,
    s3eKeyG = SDLK_g,
    s3eKeyH = SDLK_h,
    s3eKeyI = SDLK_i,
    s3eKeyJ = SDLK_j,
    s3eKeyK = SDLK_k,
    s3eKeyL = SDLK_l,
    s3eKeyM = SDLK_m,
    s3eKeyN = SDLK_n,
    s3eKeyO = SDLK_o,
    s3eKeyP = SDLK_p,
    s3eKeyQ = SDLK_q,
    s3eKeyR = SDLK_r,
    s3eKeyS = SDLK_s,
    s3eKeyT = SDLK_t,
    s3eKeyU = SDLK_u,
    s3eKeyV = SDLK_v,
    s3eKeyW = SDLK_w,
    s3eKeyX = SDLK_x,
    s3eKeyY = SDLK_y,
    s3eKeyZ = SDLK_z,
};

int mouseX, mouseY;
s3ePointerState etatMouse = S3E_POINTER_AVAILABLE;

inline int s3ePointerGetInt (s3ePointerState param) {
    if (param == S3E_POINTER_AVAILABLE) {
        int x, y;
        Uint32 mouseState = SDL_GetMouseState(&x, &y);

        if (mouseState != 0) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        std::cerr << "Parametre à renvoyer non défini. Veuillez redéfinir le paramètre." << std::endl;
        return -1;
    }
}

inline int s3ePointerGetX() {
    /*int pointerX, pointerY;
    
    // Récupérer la position actuelle de la souris
    SDL_GetMouseState(&pointerX, &pointerY);*/
    return mouseX;
}

inline int s3ePointerGetY() {
    /*int pointerX, pointerY;
    
    // Récupérer la position actuelle de la souris
    SDL_GetMouseState(&pointerX, &pointerY);*/
    return mouseY;
}

inline void s3ePointerUpdate() {
    Uint32 mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

    // Afficher la position actuelle du pointeur
    //std::cout << "Position du pointeur : X = " << mouseX << ", Y = " << mouseY << std::endl;

    // Vérifier l'état des boutons
    if (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        std::cout << "Le bouton gauche de la souris est enfoncé." << std::endl;
    }
    if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        std::cout << "Le bouton droit de la souris est enfoncé." << std::endl;
    }
}

inline void s3eKeyboardUpdate() {
    SDL_Event event;
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    // Vérifier si une touche spécifique est enfoncée (par exemple, la touche espace)
    if (keyboardState[SDL_SCANCODE_SPACE]) {
        std::cout << "La touche ESPACE est enfoncée." << std::endl;
    }

    // Gérer les événements liés au clavier
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            std::cout << "Touche enfoncée : " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        } else if (event.type == SDL_KEYUP) {
            std::cout << "Touche relâchée : " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
        }
        if (event.type == SDL_MOUSEWHEEL) {
            if (event.wheel.y < 0) {
                etatMouse = S3E_POINTER_BUTTON_MOUSEWHEELDOWN;
                std::cout << "Défilement vers le bas détecté." << std::endl;
            } else if (event.wheel.y > 0) {
                etatMouse = S3E_POINTER_BUTTON_MOUSEWHEELUP;
                std::cout << "Défilement vers le haut détecté." << std::endl;
            }
        }
    }
}

inline bool s3ePointerGetState(s3ePointerState etat) {
    if (etat == etatMouse)
    {
        return true;
    }
    else {
        std::cout << "Etat non egal à celui voulu" << std::endl;
        return false;
    }
}

inline s3ePointerState s3ePointerGetTouchState(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return S3E_POINTER_STATE_UNKNOWN;
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);
    if (finger) {
        return S3E_POINTER_STATE_DOWN;
    }

    return S3E_POINTER_STATE_UP;
}

inline float s3ePointerGetTouchX(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return -1;
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);
    if (finger) {
        float touchX = finger->x * IwGxGetScreenWidth();       // Normaliser en fonction de la largeur
        float touchY = (1.0f - finger->y) * IwGxGetScreenHeight(); // Normaliser en fonction de la hauteur
        return touchX;
    }

    return -1;
}

inline float s3ePointerGetTouchY(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return -1;
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);
    if (finger) {
        float touchX = finger->x * IwGxGetScreenWidth();       // Normaliser en fonction de la largeur
        float touchY = (1.0f - finger->y) * IwGxGetScreenHeight(); // Normaliser en fonction de la hauteur
        return touchY;
    }

    return -1;
}




inline int s3eKeyboardGetState(SDL_Keycode key) {
    // Met à jour l'état du clavier
    SDL_PumpEvents();

    // Récupère l'état de la touche spécifiée
    const Uint8* keyStates = SDL_GetKeyboardState(nullptr);
    return keyStates[SDL_GetScancodeFromKey(key)];
}

#endif