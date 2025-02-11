#ifndef FONCTIONSMARMELADE_H
#define FONCTIONSMARMELADE_H

//#include "Entete.h"

#include "ClassesMarmelade.h"
//#include "Helpers.h"
//#include "ClassesUIMarmelade.h"
#include "ClassesUIMarmelade.h"
#include "Vecteurs.h"
#include "RenderManager.h"
#include "Entete.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_locale.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL2/SDL_syswm.h>
#include <thread>
#include "gamepad_interface.h"

#if defined(_WIN32) || defined(_WIN64)  // Windows
    #include <windows.h>
    #include <shellscalingapi.h>
    #pragma comment(lib, "Shcore.lib")
    #include <iostream>
    #include <sstream>

    inline int getWindowsVersion() {
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

    inline int getMacOSVersion() {
        struct utsname buffer;
        if (uname(&buffer) == 0) {
            int major = 0, minor = 0;
            sscanf(buffer.release, "%d.%d", &major, &minor);
            return (major << 16) | minor;
        }
        return 0; // Erreur
    }
    inline long getTotalMemoryIOS() {
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
        inline float GetDPIFromXft() {
            FILE* pipe = popen("xrdb -query | grep Xft.dpi | awk '{print $2}'", "r");
            if (!pipe) return 96; // Valeur par défaut

            float dpi = 96;
            fscanf(pipe, "%f", &dpi);
            pclose(pipe);

            return dpi;
        }

        inline int dpiGetScreenDPI() {
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

    inline int getLinuxVersion() {
        struct utsname buffer;
        if (uname(&buffer) == 0) {
            int major = 0, minor = 0;
            sscanf(buffer.release, "%d.%d", &major, &minor);
            return (major << 16) | minor;
        }
        return 0; // Erreur
    }

    inline long getTotalMemoryLinux() {
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
    inline int getAndroidVersion(JNIEnv* env) {
        jclass buildClass = env->FindClass("android/os/Build$VERSION");
        jfieldID sdkIntField = env->GetStaticFieldID(buildClass, "SDK_INT", "I");
        jint sdkInt = env->GetStaticIntField(buildClass, sdkIntField);
        return (sdkInt << 16); // Android utilise seulement la version majeure
    }
    inline long getTotalMemoryAndroid() {
        // Implémentation Android spécifique
        // Cela pourrait être une interaction avec Java (JNI) pour obtenir la mémoire totale
        return 1000000000L; // Exemple de mémoire (1GB)
    }*
    
#else
    #error "OS non supporté"
#endif











// Tableau associant les noms des systèmes d'exploitation
const char* OS_NAMES[] = {
    "Windows",
    "Mac OS X",
    "Linux",
    "Android",
    "iOS",
    "Unknown"
};
// OS_NAMES[currentOS]
// Si on veut recupérer le nom de l'os en chaine de charactère


inline void s3eDebugErrorShow(errors Erreur, const char* message) {
    if (Erreur == S3E_MESSAGE_CONTINUE)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error",message,nullptr); // nullptr pour associer la boîte à la fenêtre principale, si nécessaire
    }
}

inline int s3eGLGetInt (gl param) {
    if (param == S3E_GL_VERSION) {
        GLint version;
        glGetIntegerv(GL_VERSION, &version);
        return version;
    }

    else {
        std::cerr << "Parametre GL inconnu" << std::endl;
        return -1;
    }
}

inline int s3eDeviceGetInt(fonction param) {

    if (param == S3E_DEVICE_MEM_TOTAL) {
        int osName =  s3eDeviceGetInt(S3E_DEVICE_OS);

        if (osName == S3E_OS_ID_ANDROID)
        {
            // Appeler la fonction Android spécifique
            return 0;//getTotalMemoryAndroid();
        }
        else if (osName == S3E_OS_ID_ANDROID)
        {
            // Appeler la fonction pour iOS
            return 0;//getTotalMemoryIOS();
        }
        else if (osName == S3E_OS_ID_ANDROID)
        {
            // Appeler la fonction pour Windows
            return 0;//getTotalMemoryWindows();
        }
        #ifdef __linux__
        if (osName == S3E_OS_ID_LINUX)
        {
            return getTotalMemoryLinux();
        }
        #endif
        else {
            // Sur d'autres plateformes, retourner une valeur par défaut
            return 0;
        }
    }

    else if (param == S3E_DEVICE_LANGUAGE) {
        // Obtenir les langues préférées
        SDL_Locale* locales = SDL_GetPreferredLocales();
        if (locales == nullptr) {
            std::cerr << "Impossible de récupérer les langues : " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        if (locales->language && strcmp(locales->language, "fr") == 0)
        {
            return S3E_DEVICE_LANGUAGE_FRENCH;
        }
        else if (locales->language && strcmp(locales->language, "en") == 0)
        {
            return S3E_DEVICE_LANGUAGE_ENGLISH;
        }
        else if (locales->language && strcmp(locales->language, "de") == 0)
        {
            return S3E_DEVICE_LANGUAGE_GERMAN;
        }
        else if (locales->language && strcmp(locales->language, "pt") == 0)
        {
            return S3E_DEVICE_LANGUAGE_PORTUGUESE;
        }
        else 
        {
            return S3E_DEVICE_LANGUAGE_ENGLISH;
        }

        // Libérer la mémoire allouée
        SDL_free(locales);
    }

    else if (param == S3E_DEVICE_OS) {
        const char* platform = SDL_GetPlatform();

        if (strcmp(platform, "Windows") == 0) 
        {
            return S3E_OS_ID_WINDOWS;
        } 
        else if (strcmp(platform, "Mac OS X") == 0) 
        {
            return S3E_OS_ID_MACOS;
        } 
        else if (strcmp(platform, "Linux") == 0) 
        {
            return S3E_OS_ID_LINUX;
        } 
        else if (strcmp(platform, "Android") == 0) 
        {
            return S3E_OS_ID_ANDROID;
        } 
        else if (strcmp(platform, "iOS") == 0) 
        {
            return S3E_OS_ID_IOS;
        }
        else
        {
            return S3E_OS_ID_UNKNOWN;
        }
    }

    else if (param == S3E_DEVICE_OS_VERSION) {
        const char* platform = SDL_GetPlatform();
        #ifdef _WIN32
        if (strcmp(platform, "Windows") == 0) {
            return getWindowsVersion();
        }
        #endif
        #ifdef __APPLE__
        if (strcmp(platform, "Mac OS X") == 0) {
            return getMacOSVersion();
        }
        #endif
        #ifdef __linux__
        if (strcmp(platform, "Linux") == 0) {
            return getLinuxVersion(); // Kernel version 5.x
        }
        #endif 
        #ifdef __ANDROID__
        if (strcmp(platform, "Android") == 0) {
            extern JNIEnv* env;
            return getAndroidVersion(env); // Android 12
        }
        #endif 
        if (strcmp(platform, "iOS") == 0) {
            return (15 << 16); // iOS 15
        } else {
            return 0; // Inconnu
        }
    }

    else if (param == S3E_DEVICE_NUM_CPU_CORES) {
        return GetCPUCoreCount();
    }

    else {
        std::cerr << "Parametre dont on veut les specs inconnu" << std::endl;
        return -1;
    }
}


inline const char* s3eDeviceGetString(fonction param) {

    if (param == S3E_DEVICE_ID) {

    }
    else {
        std::cerr << "Parametre dont on veut les specs inconnu dans la fonction s3eDeviceGetString" << std::endl;
        const char retour [5] = "NULL";
        return retour;
    }
}

inline int s3eSurfaceGetInt (surface param) {
    if (param == S3E_SURFACE_HEIGHT) {
        int width; int height;
        if (window) {
            SDL_GetWindowSize(window, &width, &height);
        } else {
            std::cerr << "Erreur : La fenêtre SDL n'est pas valide." << std::endl;
            width = -1; height = -1;
        }
        return height;
    }

    else if (param == S3E_SURFACE_WIDTH) {
        int width; int height;
        if (window) {
            SDL_GetWindowSize(window, &width, &height);
        } else {
            std::cerr << "Erreur : La fenêtre SDL n'est pas valide." << std::endl;
            width = -1; height = -1;
        }
        return width;
    }

    else if (param == S3E_SURFACE_DEVICE_WIDTH) {
        int displayIndex = 0; // Index de l'affichage (habituellement 0 pour l'écran principal)
        SDL_DisplayMode displayMode;
        
        if (SDL_GetCurrentDisplayMode(displayIndex, &displayMode) == 0) {
            // La fonction SDL_GetCurrentDisplayMode retourne les informations sur la résolution physique de l'écran.
            return displayMode.w;  // Retourner la largeur de l'écran physique (résolution de l'appareil).
        } else {
            std::cerr << "Erreur : Impossible de récupérer les informations sur la résolution de l'écran." << std::endl;
            return -1;  // En cas d'erreur, renvoyer une valeur invalide.
        }
    }

    else if (param == S3E_SURFACE_PIXEL_TYPE) {

        SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));  // Récupérer le format du pixel
        int bitsPerPixel = pixelFormat->BitsPerPixel;
;  // Obtenir la profondeur en bits du pixel

        return bitsPerPixel;  // Retourner la profondeur des pixels
    }

    else {
        std::cerr << "Parametre dont on veut les specs inconnu" << std::endl;
        return 0;
    }
}


inline bool s3eDeviceBacklightOn() {
    //Compléter si on veut garder l'écran allumé dans tout les cas
    return true;
}

inline uint64_t s3eTimerGetMs() {
    return SDL_GetTicks();
}

inline bool IwGxDrawRectScreenSpace(Vector2* pos, Vector2* wh) {
    
    SDL_Renderer* renderer = GeneralRender;
        if (!renderer) {
            std::cerr << "Erreur : Renderer non initialisé." << std::endl;
            return false;
        }
    // Définir le rectangle à dessiner
    SDL_Rect rect;
    rect.x = static_cast<int>(pos->x); // Position X du rectangle
    rect.y = static_cast<int>(pos->y); // Position Y du rectangle
    rect.w = static_cast<int>(wh->x);  // Largeur du rectangle
    rect.h = static_cast<int>(wh->y);  // Hauteur du rectangle

    // Dessiner le rectangle sur l'écran
    if (SDL_RenderDrawRect(renderer, &rect) != 0) {
        std::cerr << "Erreur lors du dessin du rectangle : " << SDL_GetError() << std::endl;
    }

    // Remplir le rectangle (facultatif, si nécessaire)
    // SDL_RenderFillRect(renderer, &rect);

    return true;
}

// Fonction IwSafeCast générique
template <typename T>
inline T IwSafeCast(CIwResource* resource) {
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
inline bool IwGxClear(unsigned int flags = 0) {
    GLbitfield glFlags = 0;

    // Mapper les drapeaux vers OpenGL
    if (flags & IW_GX_COLOUR_BUFFER_F) {
        glFlags |= GL_COLOR_BUFFER_BIT;
    }
    if (flags & IW_GX_DEPTH_BUFFER_F) {
        glFlags |= GL_DEPTH_BUFFER_BIT;
    }

    // Appeler glClear avec les drapeaux calculés
    glClear(glFlags);

    return true;
}

inline bool IwGxFlush() {
    return true;
}

inline bool IwGxSwapBuffers(){
    SDL_GL_SwapWindow(window);
    return true;
}


inline bool IwGLSwapBuffers() {
    SDL_GL_SwapWindow(window); //////////////////////////////////////////////////// Peut etre faire direct openGL
    return true;
}

inline uint32_t IwGxGetScreenWidth() {
    int width = 0, height = 0;
    if (window) {
        SDL_GetWindowSize(window, &width, &height);
    }
    return static_cast<uint32_t>(width);
}

inline uint32_t IwGxGetScreenHeight() {
    int width = 0, height = 0;
    if (window) {
        SDL_GetWindowSize(window, &width, &height);
    }
    return static_cast<uint32_t>(height);
}

// VibrateController function
inline bool s3eVibraVibrate(int intensity, int durationMs) {
    if (SDL_Init(SDL_INIT_HAPTIC) < 0) {
        std::cerr << "Erreur lors de l'initialisation de SDL_Haptic : " << SDL_GetError() << "\n";
        return false;
    }

    SDL_Haptic* haptic = nullptr;

    // Recherche d'un périphérique haptique
    for (int i = 0; i < SDL_NumHaptics(); ++i) {
        haptic = SDL_HapticOpen(i);
        if (haptic != nullptr) {
            break;
        }
    }

    if (!haptic) {
        std::cerr << "Aucun périphérique haptique trouvé.\n";
        SDL_Quit();
        return false;
    }

    // Vérifier si le périphérique supporte les vibrations
    if (SDL_HapticRumbleInit(haptic) < 0) {
        std::cerr << "Erreur : le périphérique ne supporte pas les vibrations : " << SDL_GetError() << "\n";
        SDL_HapticClose(haptic);
        SDL_Quit();
        return false;
    }

    // Intensité entre 0 et 1 pour SDL
    float sdlIntensity = intensity / 255.0f;
    if (SDL_HapticRumblePlay(haptic, sdlIntensity, durationMs) < 0) {
        std::cerr << "Erreur lors de la lecture de la vibration : " << SDL_GetError() << "\n";
        SDL_HapticClose(haptic);
        SDL_Quit();
        return false;
    }

    SDL_Delay(durationMs); // Attendre la fin de la vibration
    SDL_HapticClose(haptic);
    SDL_Quit();
    return true;
}




std::vector<SDL_Sensor*> accelerometers;

inline int s3eAccelerometerGetInt (accelerometer param) {
    if (param == S3E_ACCELEROMETER_AVAILABLE) {
        int numSensors = SDL_NumSensors();
        int numberAcc = 0;
        for (int i = 0; i < numSensors; ++i) {
            if (SDL_SensorGetDeviceType(i) == SDL_SENSOR_ACCEL) {
                 numberAcc+=1;
            }
        }

        return numberAcc;
    }

    else {
        return -1;
    }

}

inline bool s3eAccelerometerStart() {
    // Initialiser SDL avec le sous-système des capteurs
    if (SDL_Init(SDL_INIT_SENSOR) < 0) {
        std::cerr << "Erreur lors de l'initialisation de SDL Sensor : " << SDL_GetError() << std::endl;
        return false;
    }

    // Vérifier si un accéléromètre est disponible
    int numSensors = SDL_NumSensors();
    for (int i = 0; i < numSensors; ++i) {
        if (SDL_SensorGetDeviceType(i) == SDL_SENSOR_ACCEL) {
            SDL_Sensor* sensor = SDL_SensorOpen(i);
            if (sensor) {
                std::cout << "Accéléromètre ouvert avec succès." << std::endl;
                accelerometers.push_back(sensor);
            }
        }
    }

    std::cerr << "Aucun accéléromètre trouvé." << std::endl;
    return false;
}

inline bool s3eAccelerometerStop() {
    if (accelerometers.size()<=0) {
        std::cerr << "Aucun accéléromètre trouvé." << std::endl;
        return false;
    }
    for (unsigned int i = 0; i<accelerometers.size(); i++)
    {
        if (accelerometers[i]) {
            SDL_SensorClose(accelerometers[i]); // Fermer le capteur
        }
    }
    return true;
}

inline int32_t s3eAccelerometerGetX() {
    int32_t moy = 0;
    int numberAcc = 0;
    for (unsigned int i = 0; i<accelerometers.size(); i++)
    {
        if (!accelerometers[i]) {
            std::cerr << "Accéléromètre non initialisé." << std::endl;
            return false;
        }

        SDL_SensorUpdate(); // Met à jour les données des capteurs

        float data[3];
        SDL_SensorGetData(accelerometers[i], data, 3); // Récupère les valeurs X, Y, Z

        // Convertir en int32_t
        int32_t x = static_cast<int32_t>(data[0] * 1000); // Multiplication pour les convertir en unités similaires à l'ancienne API
        int32_t y = static_cast<int32_t>(data[1] * 1000);
        int32_t z = static_cast<int32_t>(data[2] * 1000);

        moy+=x;
        numberAcc+=1;
    }

    return moy/numberAcc;
}

inline int32_t s3eAccelerometerGetY() {
    int32_t moy = 0;
    int numberAcc = 0;
    for (unsigned int i = 0; i<accelerometers.size(); i++)
    {
        if (!accelerometers[i]) {
            std::cerr << "Accéléromètre non initialisé." << std::endl;
            return false;
        }

        SDL_SensorUpdate(); // Met à jour les données des capteurs

        float data[3];
        SDL_SensorGetData(accelerometers[i], data, 3); // Récupère les valeurs X, Y, Z

        // Convertir en int32_t
        int32_t x = static_cast<int32_t>(data[0] * 1000); // Multiplication pour les convertir en unités similaires à l'ancienne API
        int32_t y = static_cast<int32_t>(data[1] * 1000);
        int32_t z = static_cast<int32_t>(data[2] * 1000);

        moy+=y;
        numberAcc+=1;
    }

    return moy/numberAcc;
}

inline int32_t s3eAccelerometerGetZ() {
    int32_t moy = 0;
    int numberAcc = 0;
    for (unsigned int i = 0; i<accelerometers.size(); i++)
    {
        if (!accelerometers[i]) {
            std::cerr << "Accéléromètre non initialisé." << std::endl;
            return false;
        }

        SDL_SensorUpdate(); // Met à jour les données des capteurs

        float data[3];
        SDL_SensorGetData(accelerometers[i], data, 3); // Récupère les valeurs X, Y, Z

        // Convertir en int32_t
        int32_t x = static_cast<int32_t>(data[0] * 1000); // Multiplication pour les convertir en unités similaires à l'ancienne API
        int32_t y = static_cast<int32_t>(data[1] * 1000);
        int32_t z = static_cast<int32_t>(data[2] * 1000);

        moy+=z;
        numberAcc+=1;
    }

    return moy/numberAcc;
}


inline s3eResult s3eExtGetHash(uint32_t extensionHash, gamepadFuncs* g_Ext, size_t size) {
    // Vérification de l'extension gamepad
    if (extensionHash == 0xe6a02e94)
    {
        // Vérifier si SDL peut gérer des périphériques gamepad
        if (SDL_NumJoysticks() > 0)
        {
            // Remplir la structure avec les pointeurs vers les fonctions correspondantes
            g_Ext->m_gamepadGetNumDevices = []() { 
                return static_cast<unsigned int>(SDL_NumJoysticks()); 
            };

            g_Ext->m_gamepadGetDeviceId = [](unsigned int deviceIndex) { 
                return deviceIndex; 
            };

            g_Ext->m_gamepadGetNumAxes = [](unsigned int deviceIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0u;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0u;
                unsigned int axesCount = SDL_JoystickNumAxes(joystick);
                SDL_JoystickClose(joystick);
                return axesCount;
            };

            g_Ext->m_gamepadGetNumButtons = [](unsigned int deviceIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0u;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0u;
                unsigned int buttonsCount = SDL_JoystickNumButtons(joystick);
                SDL_JoystickClose(joystick);
                return buttonsCount;
            };

            g_Ext->m_gamepadGetButtons = [](unsigned int deviceIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0u;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0u;
                unsigned int buttonsState = 0;
                int numButtons = SDL_JoystickNumButtons(joystick);
                for (int i = 0; i < numButtons; i++) {
                    if (SDL_JoystickGetButton(joystick, i)) {
                        buttonsState |= (1 << i);
                    }
                }
                SDL_JoystickClose(joystick);
                return buttonsState;
            };

            g_Ext->m_gamepadGetAxis = [](unsigned int deviceIndex, unsigned int axisIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0;
                if (axisIndex >= static_cast<unsigned int>(SDL_JoystickNumAxes(joystick))) {
                    SDL_JoystickClose(joystick);
                    return 0;
                }
                int axisValue = SDL_JoystickGetAxis(joystick, axisIndex);
                SDL_JoystickClose(joystick);
                return axisValue;
            };

            g_Ext->m_gamepadIsPointOfViewAvailable = [](unsigned int deviceIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0u;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0u;
                unsigned int povAvailable = SDL_JoystickHasRumble(joystick);
                SDL_JoystickClose(joystick);
                return povAvailable;
            };

            g_Ext->m_gamepadGetPointOfViewAngle = [](unsigned int deviceIndex) { 
                if (deviceIndex >= SDL_NumJoysticks())
                    return 0;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (!joystick)
                    return 0;
                int povValue = SDL_JoystickGetHat(joystick, 0);  // Utilisation du premier D-pad
                SDL_JoystickClose(joystick);
                return povValue;
            };

            g_Ext->m_gamepadUpdate = []() { 
                SDL_PumpEvents(); 
            };

            g_Ext->m_gamepadReset = [](/*unsigned int deviceIndex*/) { 
                // Simule une réinitialisation en fermant et rouvrant le joystick
                /*if (deviceIndex >= SDL_NumJoysticks())
                    return;
                SDL_Joystick* joystick = SDL_JoystickOpen(deviceIndex);
                if (joystick)
                    SDL_JoystickClose(joystick);*/
            };

            g_Ext->m_gamepadCalibrate = [](/*unsigned int deviceIndex*/) { 
                // La calibration spécifique peut être implémentée ici si nécessaire
            };

            return S3E_RESULT_SUCCESS;
        }
        else
        {
            std::cerr << "No gamepad detected!" << std::endl;
            return S3E_RESULT_ERROR;
        }
    }

    // Si l'extension ne correspond pas, retour d'erreur
    return S3E_RESULT_ERROR;
}




// Structure représentant une extension enregistrée
struct ExtensionInfo {
    std::string name;
    std::vector<void*> functionPointers;
    std::vector<int> flags;
    s3eResult (*initFunc)();
    void (*terminateFunc)();
};

// Map pour stocker les extensions enregistrées
std::map<std::string, ExtensionInfo> g_ExtensionRegistry;

// Fonction pour enregistrer une extension
inline void s3eEdkRegister(const std::string& extensionName, void* funcPtrs[], size_t funcSize, int flags[], s3eResult (*initFunc)(), void (*terminateFunc)(), void* userData) {
    // Vérifier si l'extension existe déjà
    if (g_ExtensionRegistry.find(extensionName) != g_ExtensionRegistry.end()) {
        std::cerr << "Extension already registered: " << extensionName << std::endl;
        return;
    }

    // Créer une nouvelle entrée pour l'extension
    ExtensionInfo extInfo;
    extInfo.name = extensionName;
    extInfo.functionPointers.assign(funcPtrs, funcPtrs + (funcSize / sizeof(void*)));
    extInfo.flags.assign(flags, flags + (funcSize / sizeof(void*)));
    extInfo.initFunc = initFunc;
    extInfo.terminateFunc = terminateFunc;

    // Ajouter au registre
    g_ExtensionRegistry[extensionName] = extInfo;

    // Appeler la fonction d'initialisation, si elle est définie
    if (initFunc) {
        initFunc();
    }

    std::cout << "Extension registered: " << extensionName << std::endl;
}


inline EGLNativeWindowType s3eGLGetNativeWindow() {
SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        std::cerr << "Erreur : " << SDL_GetError() << std::endl;
        return 0;
    }

#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    return (EGLNativeWindowType)wmInfo.info.win.window; // HWND pour Windows
#elif defined(SDL_VIDEO_DRIVER_X11)
    return (EGLNativeWindowType)wmInfo.info.x11.window; // X11 Window pour Linux
#elif defined(SDL_VIDEO_DRIVER_ANDROID)
    return (EGLNativeWindowType)wmInfo.info.android.window; // ANativeWindow* pour Android
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
    return (EGLNativeWindowType)wmInfo.info.wl.surface; // wl_surface* pour Wayland
#else
    std::cerr << "Plateforme non supportée." << std::endl;
    return nullptr;
#endif
}


inline void s3eDeviceYield(int milliseconds = 50) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


inline bool s3eDeviceCheckQuitRequest() {
    SDL_Event e;
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) {
        return true;
    }
    else {
        return false;
    }
}

inline bool IwGxPrintSetColour(int r, int g, int b) {
    SDL_SetRenderDrawColor(GeneralRender, r, g, b, 255);
    return true;
}

inline bool IwGxSetColClear(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(GeneralRender, r, g, b, a);
    return true;
}


inline void* s3eMalloc(size_t size) {
    return malloc(size);
}

inline void* s3eRealloc(void* ptr, size_t size) {
    return realloc(ptr, size);
}

inline void s3eFree(void* ptr) {
    free(ptr);
}


inline void s3eOSExecExecute (std::string lien, bool val) {
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "start ";  // Sur Windows
#elif defined(__linux__)
    std::string command = "xdg-open ";  // Sur Linux
#elif defined(__APPLE__)
    std::string command = "open ";  // Sur macOS
#endif
    std::system((command + lien).c_str());
}





enum MemoryOption {
    S3E_MEMORY_HEAP = 0,
    // D'autres options mémoire peuvent être ajoutées ici
};

inline void s3eMemorySetInt(MemoryOption option, int value) {
    switch(option) {
        case S3E_MEMORY_HEAP:
            // Ici, on pourrait activer un tas mémoire ou effectuer une gestion
            // spécifique du tas mémoire
            if (value == 1) {
                std::cout << "Heap memory enabled." << std::endl;
                // Logique pour activer le tas mémoire ici
            } else {
                std::cout << "Heap memory disabled." << std::endl;
                // Logique pour désactiver le tas mémoire ici
            }
            break;
        // D'autres cas pour d'autres options mémoire peuvent être ajoutés
        default:
            std::cerr << "Unknown memory option." << std::endl;
            break;
    }
}



inline bool IwGLExtAvailable(GLextension extension) {
    std::string ext = "NULL";

    if (extension == IW_GL_OES_framebuffer_object) {
        std::string ext = "GL_OES_framebuffer_object";
    }
    else {
        printf("Extension GL non connues dans IwGLExtAvaible");
    }
    // Vérifie si l'extension est disponible dans la liste des extensions OpenGL
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (extensions && strstr(extensions, ext.c_str()) != nullptr) {
        return true;  // L'extension est disponible
    }
    return false;  // L'extension n'est pas disponible
}



/*inline int16_t ClipToInt16(float value) {
    if (value > 32767.0f) {
        return 32767;  // Valeur maximale pour int16_t
    } else if (value < -32768.0f) {
        return -32768;  // Valeur minimale pour int16_t
    } else {
        return static_cast<int16_t>(value);  // Retourner la valeur telle quelle si dans la plage
    }
}*/


#endif