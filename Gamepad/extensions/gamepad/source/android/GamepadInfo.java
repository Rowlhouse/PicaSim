package source.android;

import java.util.ArrayList;
import java.util.List;
import android.view.InputDevice;
import android.view.InputDevice.MotionRange;
import android.view.MotionEvent;
import java.lang.Float;
import android.view.KeyEvent;
import java.lang.String;

public class GamepadInfo
{
	public static ArrayList<GamepadInfo> Instance;

	List<InputDevice.MotionRange> motionsRanges;	
	public int[] axisIds;	
	public float[] axis;	
	public int buttons;	
	public InputDevice device;

	public static void onGenericMotionEvent (MotionEvent event)
	{
		if (Instance == null)
			return;
		for (int i=0; i<Instance.size(); ++i)
		{
			GamepadInfo info = Instance.get(i);
			if (info != null && info.device.getId() == event.getDeviceId())
			{
				info.handleGenericMotionEvent(event);
			}
		}
	}
	public static void onKeyUp (int keyCode, KeyEvent event)
	{
		if (Instance == null)
			return;
		for (int i=0; i<Instance.size(); ++i)
		{
			GamepadInfo info = Instance.get(i);
			if (info != null && info.device.getId() == event.getDeviceId())
			{
				info.handleKeyUp(keyCode,event);
			}
		}
	}
	public static void onKeyDown (int keyCode, KeyEvent event)
	{
		if (Instance == null)
			return;
		for (int i=0; i<Instance.size(); ++i)
		{
			GamepadInfo info = Instance.get(i);
			if (info != null && info.device.getId() == event.getDeviceId())
			{
				info.handleKeyDown(keyCode,event);
			}
		}
	}

	public GamepadInfo(InputDevice device)
	{
		this.device = device;
		this.motionsRanges = device.getMotionRanges();

		int numAxes = 0;
		for (MotionRange range : this.motionsRanges) {
                	if ((range.getSource() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0) {
	                    numAxes += 1;
        		}
		}

		this.axisIds = new int[numAxes];
		this.axis = new float[numAxes];
		int i = 0;
		for (MotionRange range : this.motionsRanges) {
			if ((range.getSource() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0) {
				this.axis[i] = 0;
				this.axisIds[i++] = range.getAxis();
			}
		}

		buttons = 0;
	}
	        
	public int GetNumAxes()
	{
		return this.axisIds.length;
	}
	public void handleGenericMotionEvent(MotionEvent event)
	{
		for (int i=0; i<this.axisIds.length; ++i)
		{
			this.axis[i] = (event.getAxisValue(this.axisIds[i]));
		}
	}

	public void handleKeyUp(int keyCode, KeyEvent event)
	{
		int newButtons = buttons;
		switch (keyCode)
		{
		case KeyEvent.KEYCODE_DPAD_DOWN:
			break;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			break;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			newButtons &= ~(1 << 31);
			break;
		case KeyEvent.KEYCODE_BUTTON_1:	// Key code constant: Generic Game Pad Button #1.
			newButtons &= ~(1 << 0);
			break;
		case KeyEvent.KEYCODE_BUTTON_2:	// Key code constant: Generic Game Pad Button #2.
			newButtons &= ~(1 << 1);
			break;
		case KeyEvent.KEYCODE_BUTTON_3:	// Key code constant: Generic Game Pad Button #3.
			newButtons &= ~(1 << 2);
			break;
		case KeyEvent.KEYCODE_BUTTON_4:	// Key code constant: Generic Game Pad Button #4.
			newButtons &= ~(1 << 3);
			break;
		case KeyEvent.KEYCODE_BUTTON_5:	// Key code constant: Generic Game Pad Button #5.
			newButtons &= ~(1 << 4);
			break;
		case KeyEvent.KEYCODE_BUTTON_6:	// Key code constant: Generic Game Pad Button #6.
			newButtons &= ~(1 << 5);
			break;
		case KeyEvent.KEYCODE_BUTTON_7:	// Key code constant: Generic Game Pad Button #7.
			newButtons &= ~(1 << 6);
			break;
		case KeyEvent.KEYCODE_BUTTON_8:	// Key code constant: Generic Game Pad Button #8.
			newButtons &= ~(1 << 7);
			break;
		case KeyEvent.KEYCODE_BUTTON_9:	// Key code constant: Generic Game Pad Button #9.
			newButtons &= ~(1 << 8);
			break;
		case KeyEvent.KEYCODE_BUTTON_10:	// Key code constant: Generic Game Pad Button #10.
			newButtons &= ~(1 << 9);
			break;
		case KeyEvent.KEYCODE_BUTTON_11:	// Key code constant: Generic Game Pad Button #11.
			newButtons &= ~(1 << 10);
			break;
		case KeyEvent.KEYCODE_BUTTON_12:	// Key code constant: Generic Game Pad Button #12.
			newButtons &= ~(1 << 11);
			break;
		case KeyEvent.KEYCODE_BUTTON_13:	// Key code constant: Generic Game Pad Button #13.
			newButtons &= ~(1 << 12);
			break;
		case KeyEvent.KEYCODE_BUTTON_14:	// Key code constant: Generic Game Pad Button #14.
			newButtons &= ~(1 << 13);
			break;
		case KeyEvent.KEYCODE_BUTTON_15:	// Key code constant: Generic Game Pad Button #15.
			newButtons &= ~(1 << 14);
			break;
		case KeyEvent.KEYCODE_BUTTON_16:	// Key code constant: Generic Game Pad Button #16.
			newButtons &= ~(1 << 15);
			break;
		case KeyEvent.KEYCODE_BUTTON_A:	// Key code constant: A Button key.
			newButtons &= ~(1 << 16);
			break;
		case KeyEvent.KEYCODE_BUTTON_B:	// Key code constant: B Button key.
			newButtons &= ~(1 << 17);
			break;
		case KeyEvent.KEYCODE_BUTTON_C:	// Key code constant: C Button key.
			newButtons &= ~(1 << 18);
			break;
		case KeyEvent.KEYCODE_BUTTON_L1:	// Key code constant: L1 Button key.
			newButtons &= ~(1 << 19);
			break;
		case KeyEvent.KEYCODE_BUTTON_L2:	// Key code constant: L2 Button key.
			newButtons &= ~(1 << 20);
			break;
		case KeyEvent.KEYCODE_BUTTON_MODE:	// Key code constant: Mode Button key.
			newButtons &= ~(1 << 21);
			break;
		case KeyEvent.KEYCODE_BUTTON_R1:	// Key code constant: R1 Button key.
			newButtons &= ~(1 << 22);
			break;
		case KeyEvent.KEYCODE_BUTTON_R2:	// Key code constant: R2 Button key.
			newButtons &= ~(1 << 23);
			break;
		case KeyEvent.KEYCODE_BUTTON_SELECT:	// Key code constant: Select Button key.
			newButtons &= ~(1 << 24);
			break;
		case KeyEvent.KEYCODE_BUTTON_START:	// Key code constant: Start Button key.
			newButtons &= ~(1 << 25);
			break;
		case KeyEvent.KEYCODE_BUTTON_THUMBL:	// Key code constant: Left Thumb Button key.
			newButtons &= ~(1 << 26);
			break;
		case KeyEvent.KEYCODE_BUTTON_THUMBR:	// Key code constant: Right Thumb Button key.
			newButtons &= ~(1 << 27);
			break;
		case KeyEvent.KEYCODE_BUTTON_X:	// Key code constant: X Button key.
			newButtons &= ~(1 << 28);
			break;
		case KeyEvent.KEYCODE_BUTTON_Y:	// Key code constant: Y Button key.
			newButtons &= ~(1 << 29);
			break;
		case KeyEvent.KEYCODE_BUTTON_Z:	// Key code constant: Z Button key.
			newButtons &= ~(1 << 30);
			break;
		}
		this.buttons = newButtons;
	}

	public void handleKeyDown(int keyCode, KeyEvent event)
	{
		int newButtons = buttons;
		switch (keyCode)
		{
		case KeyEvent.KEYCODE_DPAD_DOWN:
			break;
		case KeyEvent.KEYCODE_DPAD_LEFT:
			break;
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			break;
		case KeyEvent.KEYCODE_DPAD_CENTER:
			newButtons |= (1 << 31);
			break;
		case KeyEvent.KEYCODE_BUTTON_1:	// Key code constant: Generic Game Pad Button #1.
			newButtons |= (1 << 0);
			break;
		case KeyEvent.KEYCODE_BUTTON_2:	// Key code constant: Generic Game Pad Button #2.
			newButtons |= (1 << 1);
			break;
		case KeyEvent.KEYCODE_BUTTON_3:	// Key code constant: Generic Game Pad Button #3.
			newButtons |= (1 << 2);
			break;
		case KeyEvent.KEYCODE_BUTTON_4:	// Key code constant: Generic Game Pad Button #4.
			newButtons |= (1 << 3);
			break;
		case KeyEvent.KEYCODE_BUTTON_5:	// Key code constant: Generic Game Pad Button #5.
			newButtons |= (1 << 4);
			break;
		case KeyEvent.KEYCODE_BUTTON_6:	// Key code constant: Generic Game Pad Button #6.
			newButtons |= (1 << 5);
			break;
		case KeyEvent.KEYCODE_BUTTON_7:	// Key code constant: Generic Game Pad Button #7.
			newButtons |= (1 << 6);
			break;
		case KeyEvent.KEYCODE_BUTTON_8:	// Key code constant: Generic Game Pad Button #8.
			newButtons |= (1 << 7);
			break;
		case KeyEvent.KEYCODE_BUTTON_9:	// Key code constant: Generic Game Pad Button #9.
			newButtons |= (1 << 8);
			break;
		case KeyEvent.KEYCODE_BUTTON_10:	// Key code constant: Generic Game Pad Button #10.
			newButtons |= (1 << 9);
			break;
		case KeyEvent.KEYCODE_BUTTON_11:	// Key code constant: Generic Game Pad Button #11.
			newButtons |= (1 << 10);
			break;
		case KeyEvent.KEYCODE_BUTTON_12:	// Key code constant: Generic Game Pad Button #12.
			newButtons |= (1 << 11);
			break;
		case KeyEvent.KEYCODE_BUTTON_13:	// Key code constant: Generic Game Pad Button #13.
			newButtons |= (1 << 12);
			break;
		case KeyEvent.KEYCODE_BUTTON_14:	// Key code constant: Generic Game Pad Button #14.
			newButtons |= (1 << 13);
			break;
		case KeyEvent.KEYCODE_BUTTON_15:	// Key code constant: Generic Game Pad Button #15.
			newButtons |= (1 << 14);
			break;
		case KeyEvent.KEYCODE_BUTTON_16:	// Key code constant: Generic Game Pad Button #16.
			newButtons |= (1 << 15);
			break;
		case KeyEvent.KEYCODE_BUTTON_A:	// Key code constant: A Button key.
			newButtons |= (1 << 16);
			break;
		case KeyEvent.KEYCODE_BUTTON_B:	// Key code constant: B Button key.
			newButtons |= (1 << 17);
			break;
		case KeyEvent.KEYCODE_BUTTON_C:	// Key code constant: C Button key.
			newButtons |= (1 << 18);
			break;
		case KeyEvent.KEYCODE_BUTTON_L1:	// Key code constant: L1 Button key.
			newButtons |= (1 << 19);
			break;
		case KeyEvent.KEYCODE_BUTTON_L2:	// Key code constant: L2 Button key.
			newButtons |= (1 << 20);
			break;
		case KeyEvent.KEYCODE_BUTTON_MODE:	// Key code constant: Mode Button key.
			newButtons |= (1 << 21);
			break;
		case KeyEvent.KEYCODE_BUTTON_R1:	// Key code constant: R1 Button key.
			newButtons |= (1 << 22);
			break;
		case KeyEvent.KEYCODE_BUTTON_R2:	// Key code constant: R2 Button key.
			newButtons |= (1 << 23);
			break;
		case KeyEvent.KEYCODE_BUTTON_SELECT:	// Key code constant: Select Button key.
			newButtons |= (1 << 24);
			break;
		case KeyEvent.KEYCODE_BUTTON_START:	// Key code constant: Start Button key.
			newButtons |= (1 << 25);
			break;
		case KeyEvent.KEYCODE_BUTTON_THUMBL:	// Key code constant: Left Thumb Button key.
			newButtons |= (1 << 26);
			break;
		case KeyEvent.KEYCODE_BUTTON_THUMBR:	// Key code constant: Right Thumb Button key.
			newButtons |= (1 << 27);
			break;
		case KeyEvent.KEYCODE_BUTTON_X:	// Key code constant: X Button key.
			newButtons |= (1 << 28);
			break;
		case KeyEvent.KEYCODE_BUTTON_Y:	// Key code constant: Y Button key.
			newButtons |= (1 << 29);
			break;
		case KeyEvent.KEYCODE_BUTTON_Z:	// Key code constant: Z Button key.
			newButtons |= (1 << 30);
			break;
		}
		this.buttons = newButtons;
	}
}