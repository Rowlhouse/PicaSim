/*
* This file is part of the Marmalade SDK Code Samples.
*
* (C) 2001-2012 Marmalade. All Rights Reserved.
*
* This source code is intended only as a supplement to the Marmalade SDK.
*
* THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*/

#include "IwGx.h"
#include "gamepad.h"
#include <s3eTypes.h>
#include <s3ePointer.h>

bool g_gameLoopOn = true;
bool g_resetList = false;

void HandleClick(int x, int y)
{
  if (y < 32)
  {
    if (x > (int)IwGxGetScreenWidth() - 32)
    {
      g_gameLoopOn = false;
    }
    if (x < 32)
    {
      g_resetList = true;
    }
  }
}

S3E_API int32 pointerButtonCallback(s3ePointerEvent* ptr, void* userData)
{
  if (!ptr->m_Pressed)
  {
    HandleClick(ptr->m_x, ptr->m_y);
  }
  return 0;
}
S3E_API int32 pointerMotionCallback( s3ePointerMotionEvent* ptr, void* userData)
{
  return 0;
}
S3E_API int32 pointerTouchCallback( s3ePointerTouchEvent* ptr, void* userData)
{
  if (!ptr->m_Pressed)
  {
    HandleClick(ptr->m_x, ptr->m_y);
  }
  return 0;
}
S3E_API int32 pointerTouchMotionCallback( s3ePointerTouchMotionEvent* ptr, void* userData)
{
  return 0;
}

// Standard C-style entry point. This can take args if required.
int main()
{
  char buf [4096];
  uint32 i,j;
  uint32 num;

  s3eBool isAvailable = gamepadAvailable();

  num = gamepadGetNumDevices();

  // Initialise the IwGx drawing module
  IwGxInit();


  s3ePointerRegister(S3E_POINTER_BUTTON_EVENT,(s3eCallback)pointerButtonCallback,0);
  s3ePointerRegister(S3E_POINTER_MOTION_EVENT ,(s3eCallback)pointerMotionCallback,0);
  s3ePointerRegister(S3E_POINTER_TOUCH_EVENT ,(s3eCallback)pointerTouchCallback,0);
  s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT ,(s3eCallback)pointerTouchMotionCallback,0);

  // Set the background colour to (opaque) blue
  IwGxSetColClear(0, 0, 0xff, 0xff);
  // Loop forever, until the user or the OS performs some action to quit the app
  while (!s3eDeviceCheckQuitRequest() && g_gameLoopOn)
  {
    //if (g_resetList)
    {
      if (gamepadAvailable())
      {
        gamepadReset();
        num = gamepadGetNumDevices();
        //gamepadInit();
      }
      g_resetList = false;
    }
    // Clear the surface
    IwGxClear();

    if (!gamepadAvailable())
    {
      IwGxPrintString(100, 10, "GamePad API is not available!");
    }
    else
    {
      gamepadUpdate();
      buf[0] = 0;
	  if (num == 0)
	  {
		  IwGxPrintString(100, 10, "Zero gamepad devices!");
	  }
	  for (i=0; i<num; ++i)
      {
        sprintf(buf+strlen(buf), "%d: id=0x%08X\n", i, gamepadGetDeviceId(i));
        uint32 numAxes  = gamepadGetNumAxes(i);
        sprintf(buf+strlen(buf), "%d axes\n", numAxes);
        for (j=0; j<numAxes && j<8; ++j)
        {
          float v = gamepadGetAxis(i,j)/4096.0f;
          sprintf(buf+strlen(buf), "axis %d: %f\n", j, (double)v);
        }
        if (gamepadIsPointOfViewAvailable(i))
        {
          sprintf(buf+strlen(buf), "Point of view: %d\n", gamepadGetPointOfViewAngle(i));
        }
        uint32 numButtons = gamepadGetNumButtons(i);
        uint32 b = gamepadGetButtons(i);
        sprintf(buf+strlen(buf), "%d buttons\n", numButtons);
        for (j=0; j<numButtons && j<32; ++j)
        {
          sprintf(buf+strlen(buf), (b&1)?"X":"-");
          b = b >> 1;
        }
        sprintf(buf+strlen(buf), "\n");
      }

      IwGxPrintString(100, 10, buf);
    }

    // Standard EGL-style flush of drawing to the surface
    IwGxFlush();

    // Standard EGL-style flipping of double-buffers
    IwGxSwapBuffers();

    // Sleep for 0ms to allow the OS to process events etc.
    s3eDeviceYield(0);
  }

  // Shut down the IwGx drawing module
  IwGxTerminate();

  // Return
  return 0;
}
