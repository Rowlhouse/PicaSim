#ifndef FONCTIONSMARMELADE_H
#define FONCTIONSMARMELADE_H

// #include "Helpers.h"
// #include "ClassesUIMarmelade.h"
// #include "RenderManager.h"
// #include "Entete.h"
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_locale.h>
// #include <cassert>
// #include <chrono>
// #include <cstdint>
// #include <iostream>
// #include <fstream>
#include <SDL2/SDL_syswm.h>
#include <string>
// #include <thread>
#include "ClassesMarmelade.h"
#include "Vecteurs.h"
#include "gamepad_interface.h"

extern uint64_t frameStart;
extern uint64_t frameEnd;
extern double freq;

#if defined(_WIN32) || defined(_WIN64)  // Windows
    #include <windows.h>
    #include <shellscalingapi.h>
    #pragma comment(lib, "Shcore.lib")
    #include <iostream>
    #include <sstream>

    int getWindowsVersion() {
        OSVERSIONINFO osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        if (GetVersionEx(&osvi)) {
            return (osvi.dwMajorVersion << 16) | osvi.dwMinorVersion;
        }
        return 0; // Erreur
    }
    long getTotalMemoryWindows() {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys; // Mémoire totale physique
    }

    int GetScreenDPI() {
        HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
        UINT dpiX, dpiY;
        
        if (GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) {
            return dpiX; // dpiX = dpiY en général
        }

        // Méthode alternative pour Windows 7
        HDC screen = GetDC(0);
        int dpi = GetDeviceCaps(screen, LOGPIXELSX);
        ReleaseDC(0, screen);
        return dpi;
    }

    int GetCPUCoreCount() {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
    }
    
    std::string GetDeviceID() {
        char buffer[128];
        std::string result = "";
        FILE* pipe = _popen("wmic csproduct get UUID", "r"); // Exécute la commande WMIC
        if (!pipe) return "UNKNOWN";

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        _pclose(pipe);

        size_t pos = result.find("UUID");
        if (pos != std::string::npos) {
            result = result.substr(pos + 4); // Ignore "UUID" au début
        }

        // Nettoyage des espaces et retours à la ligne
        result.erase(remove(result.begin(), result.end(), '\n'), result.end());
        result.erase(remove(result.begin(), result.end(), '\r'), result.end());
        result.erase(remove(result.begin(), result.end(), ' '), result.end());

        return result.empty() ? "UNKNOWN" : result;
    }   



#elif defined(__APPLE__)  // macOS
    #include <ApplicationServices/ApplicationServices.h>
    #include <sys/utsname.h>
    #include <unistd.h>
    #include <stdio.h>

    int getMacOSVersion() {
        struct utsname buffer;
        if (uname(&buffer) == 0) {
            int major = 0, minor = 0;
            sscanf(buffer.release, "%d.%d", &major, &minor);
            return (major << 16) | minor;
        }
        return 0; // Erreur
    }
    long getTotalMemoryIOS() {
        int mib[2] = {CTL_HW, HW_MEMSIZE};
        size_t length;
        long memSize;

        sysctl(mib, 2, &memSize, &length, NULL, 0);
        return memSize;
    }

    int GetScreenDPI() {
        CGDirectDisplayID displayID = CGMainDisplayID();
        CGSize displaySize = CGDisplayScreenSize(displayID);
        int widthPx = CGDisplayPixelsWide(displayID);
        
        if (displaySize.width > 0) {
            return static_cast<int>(widthPx * 25.4 / displaySize.width); // Conversion mm -> pouces
        }
        return 96; // Valeur par défaut
    }

    int GetCPUCoreCount() {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }
    
    std::string GetDeviceID() {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen("ioreg -rd1 -c IOPlatformExpertDevice | grep IOPlatformUUID | awk -F '\"' '{print $4}'", "r");
        if (!pipe) return "UNKNOWN";

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        return result.empty() ? "UNKNOWN" : result;
    }

#elif defined(__linux__)  // Linux (X11 et Xft)
    #include <X11/Xlib.h>
    #include <cstdlib>
    #include <sys/utsname.h>
    #include <unistd.h>

    namespace DPI {
        void dpiInit ();

        float GetDPIFromXft();

        int dpiGetScreenDPI();

        void dpiTerminate();
    }

    int getLinuxVersion();

    long getTotalMemoryLinux();

    int GetCPUCoreCount();

    std::string GetDeviceID();

#elif defined(__ANDROID__)
    #include <jni.h>
    int getAndroidVersion(JNIEnv* env) {
        jclass buildClass = env->FindClass("android/os/Build$VERSION");
        jfieldID sdkIntField = env->GetStaticFieldID(buildClass, "SDK_INT", "I");
        jint sdkInt = env->GetStaticIntField(buildClass, sdkIntField);
        return (sdkInt << 16); // Android utilise seulement la version majeure
    }
    long getTotalMemoryAndroid() {
        // Implémentation Android spécifique
        // Cela pourrait être une interaction avec Java (JNI) pour obtenir la mémoire totale
        return 1000000000L; // Exemple de mémoire (1GB)
    }*
    
#else
    #error "OS non supporté"
#endif











// Tableau associant les noms des systèmes d'exploitation
extern const char* OS_NAMES[6];
// OS_NAMES[currentOS]
// Si on veut recupérer le nom de l'os en chaine de charactère

void s3eDebugErrorShow(errors Erreur, const char* message);

int s3eGLGetInt (gl param);

int s3eDeviceGetInt(fonction param);

const char* s3eDeviceGetString(fonction param);

int s3eSurfaceGetInt (surface param);


bool s3eDeviceBacklightOn();

uint64_t s3eTimerGetMs();

bool IwGxDrawRectScreenSpace(Vector2* pos, Vector2* wh);

// Fonction IwSafeCast générique
template <typename T>
T IwSafeCast(CIwResource* resource) {
    if (!resource) {
        std::cerr << "Erreur : Pointeur null passé à IwSafeCast." << std::endl;
        return nullptr;
    }

    // Récupérer la ressource sous forme de CIwResource*
    if (!resource) {
        std::cerr << "Erreur : La ressource est null." << std::endl;
        return nullptr;
    }

    // Vérifier si le cast est possible
    T castedResource = dynamic_cast<T>(resource);
    if (!castedResource) {
        std::cerr << "Erreur : Le cast a échoué. Type source : " 
                  << typeid(*resource).name() << " -> Type cible : " << typeid(T).name() 
                  << std::endl;
    }

    return castedResource;
}


// Définir les drapeaux comme des constantes bitmasks
constexpr unsigned int IW_GX_COLOUR_BUFFER_F = 0x01;  // Bit 0
constexpr unsigned int IW_GX_DEPTH_BUFFER_F = 0x02;   // Bit 1

// Fonction pour effacer les buffers
bool IwGxClear(unsigned int flags = 0);

bool IwGxFlush();

bool IwGxSwapBuffers();

bool IwGLSwapBuffers();

uint32_t IwGxGetScreenWidth();

uint32_t IwGxGetScreenHeight();

// VibrateController function
bool s3eVibraVibrate(int intensity, int durationMs);

extern std::vector<SDL_Sensor*> accelerometers;

int s3eAccelerometerGetInt (accelerometer param);

bool s3eAccelerometerStart();

bool s3eAccelerometerStop();

int32_t s3eAccelerometerGetX();

int32_t s3eAccelerometerGetY();

int32_t s3eAccelerometerGetZ();

s3eResult s3eExtGetHash(uint32_t extensionHash, gamepadFuncs* g_Ext, size_t size);


// Structure représentant une extension enregistrée
struct ExtensionInfo {
    std::string name;
    std::vector<void*> functionPointers;
    std::vector<int> flags;
    s3eResult (*initFunc)();
    void (*terminateFunc)();
};

// Map pour stocker les extensions enregistrées
extern std::map<std::string, ExtensionInfo> g_ExtensionRegistry;

// Fonction pour enregistrer une extension
void s3eEdkRegister(const std::string& extensionName, void* funcPtrs[], size_t funcSize, int flags[], s3eResult (*initFunc)(), void (*terminateFunc)(), void* userData);

EGLNativeWindowType s3eGLGetNativeWindow();

void s3eDeviceYield(int milliseconds = 50);

bool s3eDeviceCheckQuitRequest();

bool IwGxPrintSetColour(int r, int g, int b);

bool IwGxSetColClear(int r, int g, int b, int a);

void* s3eMalloc(size_t size);

void* s3eRealloc(void* ptr, size_t size);

void s3eFree(void* ptr);

void s3eOSExecExecute (std::string lien, bool val);

enum MemoryOption {
    S3E_MEMORY_HEAP = 0,
    // D'autres options mémoire peuvent être ajoutées ici
};

void s3eMemorySetInt(MemoryOption option, int value);

bool IwGLExtAvailable(GLextension extension);




typedef struct {
    const char *section;
    const char *key;
    int value;
} ConfigEntry;

// Exemple de configuration (ceci pourrait être remplacé par un fichier de configuration)
extern ConfigEntry config[];

// Fonction qui simule l'appel à s3eConfigGetInt pour récupérer une valeur à partir de la configuration
int s3eConfigGetInt(const char *section, const char *key, int *value);








// Définir les événements personnalisés pour la mise en pause et la reprise
#define S3E_DEVICE_PAUSE 1
#define S3E_DEVICE_UNPAUSE 2

// Définir les types de callbacks
typedef void (*DeviceCallback)(void *data);

// Structure pour enregistrer les callbacks pour chaque type d'événement
typedef struct {
    int deviceType;
    DeviceCallback callback;
    void *data;
} DeviceEventHandler;

// Liste pour enregistrer les événements et leurs callbacks
#define MAX_HANDLERS 10
extern DeviceEventHandler handlers[MAX_HANDLERS];
extern int handlerCount;

// Fonction pour enregistrer un callback pour un événement
int s3eDeviceRegister(int deviceType, DeviceCallback callback, void *data);

// Fonction pour simuler un événement (pause ou unpause)
void triggerDeviceEvent(int deviceType);

// Exemple de callback pour la mise en pause
void pauseCallback(void *data);

// Exemple de callback pour la reprise
void unpauseCallback(void *data);










void IwUIInit();

void Iw2DInit();

void Iw2DTerminate();

void IwUITerminate();

double IW_PROFILE_NEWFRAME();
#endif