#include "FonctionsMarmelade.h"
#include "FonctionsPointerKeyboardMarmelade.h"
#include <iostream>

int s3ePointerGetInt (s3ePointerState param) {
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
    else if (param == S3E_POINTER_TYPE) {
        return S3E_POINTER_TYPE_MOUSE;
    }
    else {
        std::cerr << "Parametre à renvoyer non défini. Veuillez redéfinir le paramètre." << std::endl;
        return -1;
    }
}

int s3ePointerGetX() {
    /*int pointerX, pointerY;
    
    // Récupérer la position actuelle de la souris
    SDL_GetMouseState(&pointerX, &pointerY);*/
    return mouseX;
}

int s3ePointerGetY() {
    /*int pointerX, pointerY;
    
    // Récupérer la position actuelle de la souris
    SDL_GetMouseState(&pointerX, &pointerY);*/
    return mouseY;
}

void s3ePointerUpdate() {
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

void s3eKeyboardUpdate() {
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

bool s3ePointerGetState(s3ePointerState etat) {
    if (etat == etatMouse)
    {
        return true;
    }
    else {
        std::cout << "Etat non egal à celui voulu" << std::endl;
        return false;
    }
}

s3ePointerState s3ePointerGetTouchState(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);  // Récupère le nombre de doigts actifs
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return S3E_POINTER_STATE_UNKNOWN;  // Si l'index du doigt est invalide, retourne un état inconnu
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);  // Récupère les informations du doigt
    if (finger) {
        // Pour SDL2, on utilise la position du doigt et les événements pour savoir si le doigt est pressé ou non.
        if (finger->pressure > 0.0f) {
            return S3E_POINTER_STATE_PRESSED;  // Si le doigt a une pression > 0, il est "pressé"
        } else {
            return S3E_POINTER_STATE_DOWN;  // Si la pression est nulle, il est "relâché"
        }
    }

    return S3E_POINTER_STATE_UP;  // Si aucun doigt n'est détecté, retourne un état inconnu
}



float s3ePointerGetTouchX(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return -1;
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);
    if (finger) {
        float touchX = finger->x * IwGxGetScreenWidth();       // Normaliser en fonction de la largeur
        // float touchY = (1.0f - finger->y) * IwGxGetScreenHeight(); // Normaliser en fonction de la hauteur
        return touchX;
    }

    return -1;
}

float s3ePointerGetTouchY(int touchIndex) {
    int numFingers = SDL_GetNumTouchFingers(0);
    if (touchIndex < 0 || touchIndex >= numFingers) {
        return -1;
    }

    SDL_Finger* finger = SDL_GetTouchFinger(0, touchIndex);
    if (finger) {
        // float touchX = finger->x * IwGxGetScreenWidth();       // Normaliser en fonction de la largeur
        float touchY = (1.0f - finger->y) * IwGxGetScreenHeight(); // Normaliser en fonction de la hauteur
        return touchY;
    }

    return -1;
}




int s3eKeyboardGetState(SDL_Keycode key) {
    // Met à jour l'état du clavier
    SDL_PumpEvents();

    // Récupère l'état de la touche spécifiée
    const Uint8* keyStates = SDL_GetKeyboardState(nullptr);
    return keyStates[SDL_GetScancodeFromKey(key)];
}