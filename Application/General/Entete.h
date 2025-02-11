#ifndef ENTETE_H
#define ENTETE_H

// #include <cassert>
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_video.h>
#include <cstdarg>
#include <iostream>
// #include <fstream>
#include <map>
// #include <vector>
// #include <string>
// #include <cmath>
// #include <stdio.h>
// #include <cstdio>
// #include <cstdlib>
// #include <algorithm>
// #include <stdarg.h>
// #include <list>
// #include <cstring>
// #include <set>
// #include <unistd.h>
// #include <limits>
// #include <stdlib.h>
// #include <string.h>
// #include <malloc.h>
// #include <math.h>
// //#include <windows.h>

// #include <sys/types.h>
// #include <sys/socket.h>
// //#include <sys/sysctl.h>

#include <GL/glew.h>

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>
// #include <SDL2/SDL_image.h>

#include <EGL/egl.h>
//#include <GLES2/gl2.h>   // Pour OpenGL ES 2.0

// #include <glm/glm.hpp>
// #include <glm/gtx/norm.hpp>

#include <SDL2/SDL_stdinc.h>
#include <cassert>
#include <vector>
int ROWLHOUSE = 0;
int Rowlhouse = ROWLHOUSE;

constexpr float PI = 3.14159265358979323846f; 
#define HALF_PI    (0.5f * PI)
#define TWO_PI     (2.0f * PI)
#define TWO_PI_INV (1.0f / TWO_PI)

#define S3E_SURFACE_PIXEL_SIZE_MASK 0xFF

#define MARMALADE_VERSION_STRING_FULL "0"

#define S3E_POINTER_TOUCH_MAX 2

//#define ARRAY_TYPE std::vector
template <typename T>
using CIwArray = std::vector<T>; 

#ifndef MIN
    #define MIN( _a, _b)    ((_a) < (_b) ? (_a) : (_b))
#endif

typedef SDL_bool s3eBool;
typedef char IwChar;

s3eBool S3E_FALSE = SDL_FALSE;
s3eBool S3E_TRUE = SDL_TRUE;


SDL_Window* window = SDL_CreateWindow("Native Window Example",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);


enum s3ePointerState {
    S3E_POINTER_AVAILABLE,
    S3E_POINTER_TYPE,
    S3E_POINTER_TYPE_MOUSE,
    S3E_POINTER_BUTTON_MOUSEWHEELDOWN,
    S3E_POINTER_BUTTON_MOUSEWHEELUP,
    S3E_POINTER_STATE_DOWN,
    S3E_POINTER_STATE_UP,
    S3E_POINTER_STATE_UNKNOWN,
    S3E_POINTER_STATE_PRESSED
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



enum errors {
    S3E_MESSAGE_CONTINUE,
};

enum fonction {
    S3E_DEVICE_MEM_TOTAL,
    S3E_DEVICE_LANGUAGE,
    S3E_DEVICE_OS,
    S3E_DEVICE_OS_VERSION,
    S3E_DEVICE_NUM_CPU_CORES,
    S3E_DEVICE_ID,
};

enum languages {
    S3E_DEVICE_LANGUAGE_ENGLISH,
    S3E_DEVICE_LANGUAGE_GERMAN,
    S3E_DEVICE_LANGUAGE_PORTUGUESE,
    S3E_DEVICE_LANGUAGE_FRENCH,
};

// Définition de l'enum pour les systèmes d'exploitation
enum OS {
    S3E_OS_ID_WINDOWS,
    S3E_OS_ID_WIN10,
    S3E_OS_ID_MACOS,
    S3E_OS_ID_LINUX,
    S3E_OS_ID_ANDROID,
    S3E_OS_ID_IOS,
    S3E_OS_ID_IPHONE,
    S3E_OS_ID_UNKNOWN
};

enum accelerometer {
    S3E_ACCELEROMETER_AVAILABLE,
    S3E_ACCELEROMETER_UNAVAILABLE
};

enum surface {
    S3E_SURFACE_WIDTH,
    S3E_SURFACE_HEIGHT,
    S3E_SURFACE_DEVICE_WIDTH,
    S3E_SURFACE_PIXEL_TYPE,
};

enum gl {
    S3E_GL_VERSION,
};

enum resourceType {
    IW_GX_RESTYPE_FONT,
    IW_UI_RESTYPE_STYLESHEET,
    IW_UI_RESTYPE_TEXTURE,
};

// Définition de s3eResult
enum s3eResult
{
    S3E_RESULT_SUCCESS = 0, // Indique que l'opération a réussi
    S3E_RESULT_ERROR = 1    // Indique une erreur
};

// Codes d'erreurs
enum s3eFileError{
    S3E_FILE_SUCCESS = 0,
    S3E_FILE_ERROR_NULL_POINTER = 1,
    S3E_FILE_ERROR_OPEN_FAILED = 2,
    S3E_FILE_ERROR_NOT_OPEN = 3
};

enum s3eSocketErrors {
    S3E_SOCKET_ERR_NONE = 0,
    S3E_SOCKET_ERR_WOULDBLOCK,
    S3E_SOCKET_ERR_AGAIN = S3E_SOCKET_ERR_WOULDBLOCK,
    S3E_SOCKET_ERR_CONNRESET,
    S3E_SOCKET_ERR_INPROGRESS,
    S3E_SOCKET_ERR_UNKNOWN,
};

enum s3eMessage {
    S3E_MESSAGE_CONTINUE_STOP_IGNORE,
};

enum IwTypeMessage {
    GAMEPAD_VERBOSE,
    GAMEPAD_WARNING,
    GAMEPAD_ERROR,
    GAMEPAD_INFO
};

enum CIwEventType {
    IWUI_EVENT_BUTTON,
    IWUI_EVENT_DIALOG,
    IWUI_EVENT_CLICK,
    IWUI_EVENT_HOVER,
    IWUI_EVENT_SCROLL,
    IWUI_EVENT_INPUT,
    IWUI_EVENT_TABBAR,
    IWUI_EVENT_KEY,
    IWUI_EVENT_CHAR,
    IWUI_EVENT_FOCUS,
    IWUI_EVENT_SOFTKEY,
    IWUI_EVENT_TEXT_INPUT,
    IWUI_EVENT_SLIDER,
    IWUI_EVENT_CHECKBOX,
};

enum CIwPropertyValue {
    IW_UI_ALIGN_TOP,
    IW_UI_ALIGN_CENTRE,
    IW_UI_ALIGN_MIDDLE = IW_UI_ALIGN_CENTRE,
    IW_UI_ALIGN_CENTER = IW_UI_ALIGN_CENTRE,
    IW_UI_ALIGN_BOTTOM,
    IW_UI_ALIGN_LEFT,
};

enum IwUIDebug {
    IW_UI_DEBUG_LAYOUT_OUTLINE_F,
    IW_UI_DEBUG_ELEMENT_OUTLINE_F
};

enum GLextension {
    IW_GL_OES_framebuffer_object,
};

std::map<IwTypeMessage, std::string> MessageTypeMap = {
    {GAMEPAD_VERBOSE, "VERBOSE"},
    {GAMEPAD_WARNING, "WARNING"},
    {GAMEPAD_ERROR, "ERROR"},
    {GAMEPAD_INFO, "INFO"}
};


inline void IwAssert(int RowlHouse, bool test) {
    assert(test);
}

inline void IwAssertMsg(int RowlHouse, bool test, std::string message) {
    if (!(test)) {
        std::cerr << "Assertion failed: " << message << std::endl;
        assert(test);
    }
}

inline void s3eDebugAssertShow(s3eMessage fenetre, const std::string& message) {
    //implementation possible d'une fenetre qui demande si l'on doit s'arreter, continuer, ou ignorer
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Assertion failed: %s", message.c_str());
    std::cerr << message << std::endl;
    //SDL_Quit();
    //exit(1);
}

inline void IwTrace(IwTypeMessage type, const char* format, ...) {
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

inline void IwTrace(unsigned int type, const char* format, ...) {
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



// //General
// #include "Vecteurs.h" // Vérifier les fonctions pour avoir les bonnes traductions
// #include "File.h"

// //Languages
// #include "PicaStrings.h" //Bon

// //Files
// #include "3ds.h" //Bon
// #include "ac3d.h" //Bon

// #include "tinyxml.h"



//std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower)

#endif