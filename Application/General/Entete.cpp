#include "Entete.h"
#include <iostream>

int ROWLHOUSE = 0;
int Rowlhouse = ROWLHOUSE;

s3eBool S3E_FALSE = SDL_FALSE;
s3eBool S3E_TRUE = SDL_TRUE;

int mouseX = 0, mouseY = 0;

SDL_Window* window = SDL_CreateWindow("Native Window Example",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

s3ePointerState etatMouse = S3E_POINTER_AVAILABLE;

std::map<IwTypeMessage, std::string> MessageTypeMap = {
    {GAMEPAD_VERBOSE, "VERBOSE"},
    {GAMEPAD_WARNING, "WARNING"},
    {GAMEPAD_ERROR, "ERROR"},
    {GAMEPAD_INFO, "INFO"}
};


void IwAssert(int RowlHouse, bool test) {
    assert(test);
}

void IwAssertMsg(int RowlHouse, bool test, std::string message) {
    if (!(test)) {
        std::cerr << "Assertion failed: " << message << std::endl;
        assert(test);
    }
}

void s3eDebugAssertShow(s3eMessage fenetre, const std::string& message) {
    //implementation possible d'une fenetre qui demande si l'on doit s'arreter, continuer, ou ignorer
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Assertion failed: %s", message.c_str());
    std::cerr << message << std::endl;
    //SDL_Quit();
    //exit(1);
}

void IwTrace(IwTypeMessage type, const char* format, ...) {
    // Obtenir l'étiquette correspondant au type
    std::string typeLabel = MessageTypeMap[type];

    // Extraire les arguments variables
    va_list args;
    va_start(args, format);

    // Préparer le message formaté
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end (args);

    // Afficher le message
    std::cout << "[" << typeLabel << "] " << buffer << std::endl;
}

void IwTrace(unsigned int type, const char* format, ...) {
    // Obtenir l'étiquette correspondant au type
    std::string typeLabel = "ROWLHOUSE";

    // Extraire les arguments variables
    va_list args;
    va_start(args, format);

    // Préparer le message formaté
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end (args);

    // Afficher le message
    std::cout << "[" << typeLabel << "] " << buffer << std::endl;
}