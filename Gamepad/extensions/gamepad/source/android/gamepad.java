/*
java implementation of the gamepad extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import android.view.InputDevice;
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;
import java.util.ArrayList;
import android.view.MotionEvent;
import source.android.GamepadInfo;
import java.lang.String;

class gamepad
{
	public ArrayList<InputDevice> gamepad_devices;
	
	public gamepad()
	{
	}
    public int gamepadGetNumDevices()
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			return gamepad_devices.size();
		}
		else
		{
			return 0;
		}
    }
    public int gamepadGetNumAxes(int index)
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			return GamepadInfo.Instance.get(index).GetNumAxes();
		}
		else
		{
			return 0;
		}
    }
    public int gamepadGetNumButtons(int index)
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			return 32;
		}
		else
		{
			return 0;
		}
    }
    public float gamepadGetAxis(int index, int axisIndex)
    {
		if (android.os.Build.VERSION.SDK_INT >= 11 && axisIndex < GamepadInfo.Instance.get(index).GetNumAxes())
		{
			return GamepadInfo.Instance.get(index).axis[axisIndex];
		}
		else
		{
			return 0;
		}
    }
    public int gamepadGetButtons(int index)
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			return GamepadInfo.Instance.get(index).buttons;
		}
		else
		{
			return 0;
		}
    }
    public int gamepadGetDeviceId(int index)
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			return gamepad_devices.get(index).getSources();
		}
		else
		{
			return 0;
		}
    }
    public void gamepadInit()
    {
		if (android.os.Build.VERSION.SDK_INT >= 11) 
		{
			int[] devices;

			gamepad_devices = new ArrayList<InputDevice>();

			GamepadInfo.Instance = new ArrayList<GamepadInfo>();

			devices = InputDevice.getDeviceIds();
			for (int i=0; i<devices.length; ++i)
			{
				InputDevice device = InputDevice.getDevice(devices[i]);
				int sources = device.getSources();
				if (((sources & InputDevice.SOURCE_DPAD) == InputDevice.SOURCE_DPAD)
					|| ((sources & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)
					|| ((sources & InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK))
				{
					gamepad_devices.add(device);
					GamepadInfo.Instance.add(new GamepadInfo(device));
				}
			}
		}
    }
    public void gamepadTerminate()
    {
        
    }
    public void gamepadUpdate()
    {
        
    }
}
