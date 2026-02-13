#include "Window.h"
#include "Platform.h"
#include "S3ECompat.h"
#include "../Framework/Trace.h"
#include <stdio.h>

#ifdef _WIN32
#include <glad/glad.h>
#elif defined(PICASIM_ANDROID) || defined(__ANDROID__)
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif

// Global window instance
Window* gWindow = nullptr;

Window::Window()
    : mWindow(nullptr)
    , mContext(nullptr)
    , mWidth(0)
    , mHeight(0)
    , mFullscreen(false)
    , mGlMajorVersion(0)
    , mGlMinorVersion(0)
{
}

Window::~Window()
{
    Shutdown();
}

bool Window::Init(int width, int height, const char* title, bool fullscreen, int msaaSamples)
{
    TRACE_FILE_IF(ONCE_1) TRACE("Window::Init (SDL_WasInit=0x%x)", SDL_WasInit(0));

    // Initialize SDL video subsystem if not already done
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
        TRACE_FILE_IF(ONCE_2) TRACE("Calling SDL_Init(SDL_INIT_VIDEO)");
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            TRACE_FILE_IF(ONCE_1) TRACE("SDL_Init failed: %s", SDL_GetError());
            fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }
        TRACE_FILE_IF(ONCE_2) TRACE("SDL_Init succeeded (SDL_WasInit=0x%x)", SDL_WasInit(0));
    }
    else
    {
        TRACE_FILE_IF(ONCE_2) TRACE("SDL video already initialized, skipping SDL_Init");
    }

    // Get display mode for default resolution
    SDL_DisplayMode displayMode;
    TRACE_FILE_IF(ONCE_2) TRACE("Querying desktop display mode");
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        fprintf(stderr, "SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        return false;
    }
    TRACE_FILE_IF(ONCE_1) TRACE("Desktop display mode: %dx%d", displayMode.w, displayMode.h);

    // Use desktop resolution if width/height are 0
    if (width <= 0) width = displayMode.w;
    if (height <= 0) height = displayMode.h;

    // Set OpenGL attributes before creating window
    // Request OpenGL 3.3 Core Profile for desktop, but fall back to 2.1 if needed
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
    // Mobile platforms use OpenGL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    // Desktop platforms - GL 2.1 compatibility mode
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Create window with MSAA step-down: try requested samples, then halve until success or 0
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#if defined(PICASIM_ANDROID) || defined(__ANDROID__)
    // Android: always fullscreen, not resizable
    windowFlags |= SDL_WINDOW_FULLSCREEN;
#else
    windowFlags |= SDL_WINDOW_RESIZABLE;
    if (fullscreen)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
#endif

    int requestedMsaa = msaaSamples;
    while (true)
    {
        // Set MSAA attributes
        if (msaaSamples > 0)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaaSamples);
        }
        else
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
        }

        // Try to create window
        TRACE_FILE_IF(ONCE_2) TRACE("Creating SDL window %dx%d (flags=0x%x, MSAA=%d)", width, height, windowFlags, msaaSamples);
        mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
        if (!mWindow)
        {
            if (msaaSamples > 0)
            {
                TRACE_FILE_IF(ONCE_1) TRACE("SDL_CreateWindow failed with MSAA=%d: %s - stepping down", msaaSamples, SDL_GetError());
                msaaSamples = (msaaSamples > 1) ? msaaSamples / 2 : 0;
                continue;
            }
            TRACE_FILE_IF(ONCE_1) TRACE("SDL_CreateWindow failed: %s", SDL_GetError());
            fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
            return false;
        }

        // Try to create GL context
        TRACE_FILE_IF(ONCE_2) TRACE("Creating OpenGL context");
        mContext = SDL_GL_CreateContext(mWindow);
        if (!mContext)
        {
            if (msaaSamples > 0)
            {
                TRACE_FILE_IF(ONCE_1) TRACE("SDL_GL_CreateContext failed with MSAA=%d: %s - stepping down", msaaSamples, SDL_GetError());
                SDL_DestroyWindow(mWindow);
                mWindow = nullptr;
                msaaSamples = (msaaSamples > 1) ? msaaSamples / 2 : 0;
                continue;
            }
            TRACE_FILE_IF(ONCE_1) TRACE("SDL_GL_CreateContext failed: %s", SDL_GetError());
            fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
            return false;
        }

        // Verify actual MSAA samples match what we requested
        if (msaaSamples > 0)
        {
            int actualSamples = 0;
            SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &actualSamples);
            if (actualSamples < msaaSamples)
            {
                TRACE_FILE_IF(ONCE_1) TRACE("MSAA %d requested but only %d delivered - stepping down", msaaSamples, actualSamples);
                SDL_GL_DeleteContext(mContext);
                mContext = nullptr;
                SDL_DestroyWindow(mWindow);
                mWindow = nullptr;
                msaaSamples = (msaaSamples > 1) ? msaaSamples / 2 : 0;
                continue;
            }
        }

        break; // success
    }

    if (msaaSamples != requestedMsaa)
    {
        TRACE_FILE_IF(ONCE_1) TRACE("MSAA stepped down from %d to %d", requestedMsaa, msaaSamples);
    }
    TRACE_FILE_IF(ONCE_2) TRACE("SDL window created (window=%p)", mWindow);
    TRACE_FILE_IF(ONCE_2) TRACE("OpenGL context created");

    // Make context current
    SDL_GL_MakeCurrent(mWindow, mContext);

#ifdef _WIN32
    // Initialize GLAD for OpenGL function loading on Windows
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        SDL_GL_DeleteContext(mContext);
        SDL_DestroyWindow(mWindow);
        mContext = nullptr;
        mWindow = nullptr;
        return false;
    }
#endif

    // Enable vsync (1 = enable, 0 = disable, -1 = adaptive)
    SDL_GL_SetSwapInterval(1);

    // Get actual window size (may differ from requested due to DPI scaling)
    SDL_GetWindowSize(mWindow, &mWidth, &mHeight);
    mFullscreen = fullscreen;

    // Get OpenGL version
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &mGlMajorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &mGlMinorVersion);

    // Enable multisampling if requested (not available on GLES2)
#if !defined(PICASIM_ANDROID) && !defined(__ANDROID__) && !defined(PICASIM_IOS)
    if (msaaSamples > 0)
    {
        glEnable(GL_MULTISAMPLE);
    }
#endif

    // Get actual MSAA samples (may be different from requested if not supported)
    int actualMsaaSamples = 0;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &actualMsaaSamples);

    printf("Window created: %dx%d, OpenGL %d.%d\n", mWidth, mHeight, mGlMajorVersion, mGlMinorVersion);
    printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("MSAA: requested=%d, actual=%d\n", msaaSamples, actualMsaaSamples);

    return true;
}

void Window::Shutdown()
{
    TRACE_FILE_IF(ONCE_1) TRACE("Window::Shutdown (context=%p, window=%p)", mContext, mWindow);
    if (mContext)
    {
        SDL_GL_DeleteContext(mContext);
        mContext = nullptr;
    }

    if (mWindow)
    {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    mWidth = 0;
    mHeight = 0;
    mFullscreen = false;
}

void Window::SwapBuffers()
{
    if (mWindow)
    {
        SDL_GL_SwapWindow(mWindow);
    }
}

bool Window::ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                mWidth = event.window.data1;
                mHeight = event.window.data2;
            }
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                // ESC can be used to exit fullscreen
                if (mFullscreen)
                {
                    SetFullscreen(false);
                }
            }
            break;
        }

        // TODO: Forward events to input system
    }

    return true;
}

void Window::SetFullscreen(bool fullscreen)
{
    if (mWindow && mFullscreen != fullscreen)
    {
        Uint32 flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        if (SDL_SetWindowFullscreen(mWindow, flags) == 0)
        {
            mFullscreen = fullscreen;
            SDL_GetWindowSize(mWindow, &mWidth, &mHeight);
        }
    }
}

void Window::SetTitle(const char* title)
{
    if (mWindow)
    {
        SDL_SetWindowTitle(mWindow, title);
    }
}

void Window::Resize(int width, int height)
{
    if (mWindow && !mFullscreen)
    {
        SDL_SetWindowSize(mWindow, width, height);
        mWidth = width;
        mHeight = height;
    }
}

Window& Window::GetInstance()
{
    IwAssert(ROWLHOUSE, gWindow != nullptr);
    return *gWindow;
}
