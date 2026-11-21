package com.rowlhouse.picasim;

import android.util.Log;
import android.view.KeyEvent;

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

    /**
     * Logs back key events, so logcat shows whether Android delivers them to PicaSim.
     * SDL then passes them to the native code as SDLK_AC_BACK.
     */
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
            Log.i("PicaSim", "Back key " + (event.getAction() == KeyEvent.ACTION_DOWN ? "down" : "up")
                    + " canceled=" + event.isCanceled() + " source=0x" + Integer.toHexString(event.getSource())
                    + " device=" + event.getDeviceId());
        }
        return super.dispatchKeyEvent(event);
    }
}
