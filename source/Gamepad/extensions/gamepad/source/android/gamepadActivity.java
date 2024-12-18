/*
java implementation of the gamepad extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */

package source.android;

import android.view.InputDevice;
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import java.util.ArrayList;
import android.view.MotionEvent;
import source.android.GamepadInfo;
import android.view.KeyEvent;

public class gamepadActivity extends LoaderActivity
{
	@Override
	public boolean onGenericMotionEvent (MotionEvent event)
	{
		GamepadInfo.onGenericMotionEvent(event);
		return super.onGenericMotionEvent(event);
	}

	@Override
	public boolean onKeyDown (int keyCode, KeyEvent event)
	{
		GamepadInfo.onKeyDown(keyCode, event);
		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp (int keyCode, KeyEvent event)
	{
		GamepadInfo.onKeyUp(keyCode, event);
		return super.onKeyUp(keyCode, event);
	}
}
