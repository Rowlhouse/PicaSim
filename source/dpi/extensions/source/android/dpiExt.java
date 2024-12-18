/*
java implementation of the dpiExt extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;

import android.app.Activity;
import android.os.Bundle;
import android.view.Display;
import android.view.WindowManager;
import android.widget.TextView;
import android.util.DisplayMetrics;



class dpiExt
{
    public int dpiExtGetDeviceDPI()
    {
		DisplayMetrics metrics = new DisplayMetrics();
		Activity a = LoaderActivity.m_Activity;
		if (a != null)
		{
			WindowManager wm = a.getWindowManager();
			if (wm != null)
			{
				Display d = wm.getDefaultDisplay();
				if (d != null)
				{
					d.getMetrics(metrics);
					return (int)(metrics.density*160.0f);
				}
			}
		}
		return -1;
    }
}
