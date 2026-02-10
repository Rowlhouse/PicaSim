package com.rowlhouse.picasim;

import org.libsdl.app.SDLActivity;

/**
 * PicaSim Android Activity - extends SDLActivity for SDL2 integration.
 * SDLActivity handles the native library loading, GL surface creation,
 * and event dispatching to the native C++ code.
 */
public class PicaSimActivity extends SDLActivity {

    /**
     * Called by SDL to determine which shared libraries to load.
     */
    @Override
    protected String[] getLibraries() {
        return new String[]{
            "SDL2",
            "openal",
            "PicaSim"
        };
    }
}
