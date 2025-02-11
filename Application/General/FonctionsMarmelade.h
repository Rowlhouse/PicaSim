#ifndef FONCTIONSMARMELADE_H
#define FONCTIONSMARMELADE_H

// #include "ClassesMarmelade.h"
// #include "Helpers.h"
#include "ClassesUIMarmelade.h"
// #include "Vecteurs.h"
// #include "RenderManager.h"
// #include "Entete.h"
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_locale.h>
// #include <cassert>
// #include <chrono>
// #include <cstdint>
// #include <string>
// #include <iostream>
#include <fstream>
#include <SDL2/SDL_syswm.h>
// #include <thread>
#include "gamepad_interface.h"

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
        float GetDPIFromXft() {
            FILE* pipe = popen("xrdb -query | grep Xft.dpi | awk '{print $2}'", "r");
            if (!pipe) return 96; // Valeur par défaut

            float dpi = 96;
            fscanf(pipe, "%f", &dpi);
            pclose(pipe);

            return dpi;
        }

        int dpiGetScreenDPI() {
            Display* dpy = XOpenDisplay(nullptr);
            if (!dpy) {
                std::cerr << "Erreur : Impossible d'ouvrir X11, tentative avec Xft...\n";
                return static_cast<int>(GetDPIFromXft());
            }

            int screen = DefaultScreen(dpy);
            int width_px = DisplayWidth(dpy, screen);
            int width_mm = DisplayWidthMM(dpy, screen);
            XCloseDisplay(dpy);

            if (width_mm > 0) {
                return static_cast<int>(width_px * 25.4 / width_mm);
            }

            return static_cast<int>(GetDPIFromXft());
        }
    }

    int getLinuxVersion() {
        struct utsname buffer;
        if (uname(&buffer) == 0) {
            int major = 0, minor = 0;
            sscanf(buffer.release, "%d.%d", &major, &minor);
            return (major << 16) | minor;
        }
        return 0; // Erreur
    }

    long getTotalMemoryLinux() {
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        long totalMemory = 0;

        if (meminfo.is_open())
        {
            while (std::getline(meminfo, line))
            {
                if (line.find("MemTotal:") == 0) // Rechercher la ligne "MemTotal"
                {
                    std::sscanf(line.c_str(), "MemTotal: %ld kB", &totalMemory);
                    totalMemory *= 1024; // Convertir kB en bytes
                    break;
                }
            }
            meminfo.close();
        }
        return totalMemory;
    }

    int GetCPUCoreCount() {
        return sysconf(_SC_NPROCESSORS_ONLN);
    }

    std::string GetDeviceID() {
        std::ifstream file("/etc/machine-id");
        if (!file) file.open("/var/lib/dbus/machine-id"); // Backup file

        std::string id;
        if (file) {
            std::getline(file, id);
            file.close();
        }

        return id.empty() ? "UNKNOWN" : id;
    }

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
    // Vérifier si le pointeur est nul
    if (!resource) {
        return nullptr;
    }

    // Utiliser dynamic_cast pour tenter de convertir le pointeur en un type dérivé
    T castedResource = dynamic_cast<T>(resource);
    if (!castedResource) {
        std::cerr << "Erreur : Le cast de type a échoué." << std::endl;
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

#endif