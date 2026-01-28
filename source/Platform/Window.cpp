#include "Window.h"
#include "Platform.h"
#include <stdio.h>

// TODO: fixare
#ifdef _WIN32
#if !defined(PICASIM_MACOS)
  #include <glad/glad.h>
#endif
#else
#if defined(PICASIM_MACOS)
  #include <OpenGL/gl.h>
#else
  #ifdef __APPLE__
    #include <OpenGL/gl.h>
#else
    #include "GLCompat.h"
    #endif
#endif
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
    // Initialize SDL video subsystem if not already done
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
            return false;
        }
    }

    // Get display mode for default resolution
    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
    {
        fprintf(stderr, "SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        return false;
    }

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
    // Desktop platforms - OpenGL 3.3 Compatibility Profile (supports legacy + modern GL)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    // Set MSAA attributes if requested
    if (msaaSamples > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaaSamples);
    }

    // Create window
    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (fullscreen)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    mWindow = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        windowFlags
    );

    if (!mWindow)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    // Create OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);
    if (!mContext)
    {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        
        // Try fallback: disable MSAA if it was requested
        if (msaaSamples > 0)
        {
            fprintf(stderr, "Retrying without MSAA...\n");
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            mContext = SDL_GL_CreateContext(mWindow);
        }
        
        // If still failing, try OpenGL 2.1 compatibility mode
        if (!mContext)
        {
            fprintf(stderr, "Retrying with OpenGL 2.1...\n");
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
            mContext = SDL_GL_CreateContext(mWindow);
        }
        
        if (!mContext)
        {
            fprintf(stderr, "Failed to create any OpenGL context\n");
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
            return false;
        }
    }

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

    // Enable multisampling if requested
    if (msaaSamples > 0)
    {
        glEnable(GL_MULTISAMPLE);
    }

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
    static Window instance;
    return instance;
}
