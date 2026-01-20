#ifndef WINDOW_H
#define WINDOW_H

/**
  * Window - SDL2-based window and OpenGL context management
  *
  * This class replaces the Marmalade EGL initialization (eglInit/eglTerm)
  * with SDL2 window and OpenGL context creation.
  */

#include <SDL.h>
#include <string>

class Window
{
public:
        Window();
        ~Window();

        /**
          * Initialize the window and OpenGL context.
          * @param width Window width (0 for desktop resolution)
          * @param height Window height (0 for desktop resolution)
          * @param title Window title
          * @param fullscreen Start in fullscreen mode
          * @param msaaSamples MSAA sample count (0=off, 2, 4, 8)
          * @return true on success
          */
        bool Init(int width = 0, int height = 0, const char* title = "PicaSim", bool fullscreen = false, int msaaSamples = 0);

        /**
          * Shutdown the window and release resources.
          */
        void Shutdown();

        /**
          * Swap the front and back buffers.
          */
        void SwapBuffers();

        /**
          * Process window events.
          * @return false if quit was requested
          */
        bool ProcessEvents();

        // Accessors
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }
        float GetAspectRatio() const { return mWidth > 0 ? (float)mWidth / mHeight : 1.0f; }
        bool IsFullscreen() const { return mFullscreen; }
        bool IsInitialized() const { return mWindow != nullptr; }

        // Update internal size (called on resize events)
        void SetSize(int width, int height) { mWidth = width; mHeight = height; }

        // Window manipulation
        void SetFullscreen(bool fullscreen);
        void SetTitle(const char* title);
        void Resize(int width, int height);

        // Get the SDL window handle (for advanced usage)
        SDL_Window* GetSDLWindow() const { return mWindow; }
        SDL_GLContext GetGLContext() const { return mContext; }

        // Get OpenGL version
        int GetGLMajorVersion() const { return mGlMajorVersion; }
        int GetGLMinorVersion() const { return mGlMinorVersion; }

        // Singleton access (optional)
        static Window& GetInstance();

private:
        SDL_Window* mWindow;
        SDL_GLContext mContext;
        int mWidth;
        int mHeight;
        bool mFullscreen;
        int mGlMajorVersion;
        int mGlMinorVersion;

        // Prevent copying
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
};

// Global window instance for convenience (replaces eglInit/eglTerm globals)
extern Window* gWindow;

#endif // WINDOW_H
