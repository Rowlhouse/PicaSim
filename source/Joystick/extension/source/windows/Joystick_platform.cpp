/*
* windows-specific implementation of the Joystick extension.
* Add any platform-specific functionality here.
*/
/*
* NOTE: This file was originally written by the extension builder, but will not
* be overwritten (unless --force is specified) and is intended to be modified.
*/
#define _CRT_SECURE_NO_WARNINGS


#include <InitGuid.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <IwDebug.h>
#include "Joystick_internal.h"

#pragma comment(lib, "DInput8.lib")

//---------------------------------------------------------------------------------------------------------------------
class Joystick
{
private:
  unsigned int            id;
  unsigned int            device_counter;

  LPDIRECTINPUT8          di;
  LPDIRECTINPUTDEVICE8    joystick;

public:
  Joystick(unsigned int id);
  ~Joystick();

  HRESULT deviceName(char* name);

  HRESULT open();
  HRESULT close();

  HRESULT poll(DIJOYSTATE2 *js);

  BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);

  // Device Querying
  static unsigned int deviceCount();
};

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE* instance, VOID* context);
BOOL CALLBACK countCallback(const DIDEVICEINSTANCE* instance, VOID* counter);

#define SAFE_RELEASE(p)     { if(p) { (p)->Release(); (p) = NULL; } }

//---------------------------------------------------------------------------------------------------------------------
Joystick::Joystick(unsigned int id)
{
  this->id = id;
  device_counter = 0;

  di = NULL;
  joystick = NULL;
}

//---------------------------------------------------------------------------------------------------------------------
Joystick::~Joystick()
{
  close();
}

//---------------------------------------------------------------------------------------------------------------------
HRESULT
  Joystick::deviceName(char* name)
{
  IwTrace(JOYSTICK_VERBOSE, ("Getting joystick device name"));
  HRESULT hr;
  DIDEVICEINSTANCE device;

  ZeroMemory(&device, sizeof(device));
  device.dwSize = sizeof(device);

  if (!di || !joystick) {
    return E_INVALIDARG;
  }

  if (FAILED(hr = joystick->GetDeviceInfo(&device))) {
    return hr;
  }

  strncpy(name, device.tszProductName, MAX_PATH);

  return hr;
}

//---------------------------------------------------------------------------------------------------------------------
HRESULT
  Joystick::open()
{
  HRESULT hr;

  // Create a DirectInput device
  if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
    DIRECTINPUT_VERSION, 
    IID_IDirectInput8,
    (VOID**)&di, NULL))) 
  {
    IwTrace(JOYSTICK, ("Failed to create DirectInput8"));
    return hr;
  }

  // Look for the first simple joystick we can find.
  if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, ::enumCallback,
    (LPVOID)this, DIEDFL_ATTACHEDONLY))) 
  {
    IwTrace(JOYSTICK, ("Failed to enumerate devices"));
    return hr;
  }

  // Make sure we got a joystick
  if (joystick == NULL) 
  {
    IwTrace(JOYSTICK, ("Failed to get a joystick"));
    return E_FAIL;
  }

  // Set the data format to "simple joystick" - a predefined data format 
  //
  // A data format specifies which controls on a device we are interested in,
  // and how they should be reported. This tells DInput that we will be
  // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
  if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick2))) 
  {
    IwTrace(JOYSTICK, ("Failed to set the joystick format"));
    return hr;
  }

  // Set the cooperative level to let DInput know how this device should
  // interact with the system and with other DInput applications.
  if (FAILED(hr = joystick->SetCooperativeLevel(NULL, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) 
  {
    IwTrace(JOYSTICK, ("Failed to set joystick cooperative level"));
    return hr;
  }

  return S_OK;
}

HRESULT
  Joystick::close()
{
  if (joystick) 
  { 
    IwTrace(JOYSTICK, ("Unacquiring joystick"));
    joystick->Unacquire();
  }

  SAFE_RELEASE(joystick);
  SAFE_RELEASE(di);

  return S_OK;
}

//---------------------------------------------------------------------------------------------------------------------
HRESULT
  Joystick::poll(DIJOYSTATE2 *js)
{
  HRESULT hr;

  if (joystick == NULL) 
  {
    return S_OK;
  }

  // Poll the device to read the current state
  hr = joystick->Poll(); 
  if (FAILED(hr)) 
  {
    IwTrace(JOYSTICK, ("Failed to poll joystick - attempting to re-acquire hr = %d", hr));
    
    if (hr == DIERR_INPUTLOST) 
      IwTrace(JOYSTICK, ("Input was lost"));
    else if (hr == DIERR_NOTACQUIRED)
      IwTrace(JOYSTICK, ("Input was not acquired"));
    else if (hr == DIERR_NOTINITIALIZED)
      IwTrace(JOYSTICK, ("Input was not initialised"));

    // DirectInput is telling us that the input stream has been
    // interrupted.  We aren't tracking any state between polls, so we
    // don't have any special reset that needs to be done.  We just
    // re-acquire and try again.
    hr = joystick->Acquire();
    while (hr == DIERR_INPUTLOST) 
    {
      IwTrace(JOYSTICK, ("Failed to acquire joystick - trying again as input still lost"));
      hr = joystick->Acquire();
    }

    // If we encounter a fatal error, return failure.
    if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) 
    {
      IwTrace(JOYSTICK, ("Failed to poll joystick"));
      return E_FAIL;
    }

    // If another application has control of this device, return success.
    // We'll just have to wait our turn to use the joystick.
    if (hr == DIERR_OTHERAPPHASPRIO) 
    {
      IwTrace(JOYSTICK, ("Another application has control of the joystick"));
      return S_OK;
    }
  }

  // Get the input's device state
  if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js))) 
  {
    IwTrace(JOYSTICK, ("Failed to get joystick state"));
    return hr;
  }

  return S_OK;
}

//---------------------------------------------------------------------------------------------------------------------
BOOL CALLBACK
  Joystick::enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
  // If this is the requested device ID ...
  if (device_counter == this->id) {

    // Obtain an interface to the enumerated joystick.  Stop the enumeration
    // if the requested device was created successfully.
    if (SUCCEEDED(di->CreateDevice(instance->guidInstance, &joystick, NULL))) {
      return DIENUM_STOP;
    }  
  }

  // Otherwise, increment the device counter and continue with
  // the device enumeration.
  device_counter++;

  return DIENUM_CONTINUE;
}

//---------------------------------------------------------------------------------------------------------------------
BOOL CALLBACK
  enumCallback(const DIDEVICEINSTANCE* instance, VOID* context)
{
  if (context != NULL) {
    return ((Joystick *)context)->enumCallback(instance, context);
  } else {
    return DIENUM_STOP;
  }
}

LPDIRECTINPUT8 g_directInput = NULL;

//---------------------------------------------------------------------------------------------------------------------
unsigned int
  Joystick::deviceCount()
{
  unsigned int counter = 0;
  
  if (g_directInput)
  {
    g_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, ::countCallback,
      &counter, DIEDFL_ATTACHEDONLY);
  }

  return counter;
}

//---------------------------------------------------------------------------------------------------------------------
BOOL CALLBACK
  countCallback(const DIDEVICEINSTANCE* instance, VOID* counter)
{
  if (counter != NULL) {
    unsigned int *tmpCounter = (unsigned int *)counter;
    (*tmpCounter)++;
    counter = tmpCounter;
  }

  return DIENUM_CONTINUE;
}


//---------------------------------------------------------------------------------------------------------------------
// Interface implementation
//---------------------------------------------------------------------------------------------------------------------

Joystick* s_joysticks[4] = {0,0,0,0};
unsigned int s_numJoysticks = 0;
const unsigned int maxNumJoysticks = sizeof(s_joysticks) / sizeof(s_joysticks[0]);

//---------------------------------------------------------------------------------------------------------------------
s3eResult JoystickInit_platform()
{
  IwTrace(JOYSTICK, ("Initialising joystick extension"));

  if (!g_directInput)
  {
    IwTrace(JOYSTICK, ("Creating DX8"));
    DirectInput8Create(GetModuleHandle(NULL),
      DIRECTINPUT_VERSION, 
      IID_IDirectInput8,
      (VOID**)&g_directInput, NULL);
  }

  s_numJoysticks = Joystick::deviceCount();
  if (s_numJoysticks > maxNumJoysticks)
    s_numJoysticks = maxNumJoysticks;
  IwTrace(JOYSTICK, ("Initialising %d joysticks", s_numJoysticks));
  for (unsigned int i = 0; i < s_numJoysticks; i++) 
  {
    s_joysticks[i] = new Joystick(i);
    s_joysticks[i]->open();

    // Print the name of the joystick.
    char name[MAX_PATH];
    s_joysticks[i]->deviceName(name);
    IwTrace(JOYSTICK, ("  Joystick %d: %s\n", i, name));
  }

  return S3E_RESULT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
void JoystickTerminate_platform()
{
  IwTrace(JOYSTICK, ("Terminating joystick extension"));
  for (unsigned int i = 0; i < s_numJoysticks; i++) 
  {
    if (s_joysticks[i])
    {
      delete (s_joysticks[i]);
      s_joysticks[i] = 0;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
s3eResult GetJoystickStatus_platform(JoystickData& joystick, int id)
{
  // This allows joysticks to be added/removed - but just counting them is terribly slow!
  //if (s_numJoysticks != Joystick::deviceCount())
  //{
  //  JoystickTerminate_platform();
  //  JoystickInit_platform();
  //  s_numJoysticks = Joystick::deviceCount();
  //}

  if (id >= maxNumJoysticks)
    id = maxNumJoysticks;

  if (!s_joysticks[id])
    return S3E_RESULT_ERROR;
  DIJOYSTATE2 js;
  s_joysticks[id]->poll(&js);

  IwTrace(JOYSTICK_VERBOSE, ("X Axis: %ld\n", js.lX));
  IwTrace(JOYSTICK_VERBOSE, ("Y Axis: %ld\n", js.lY));
  IwTrace(JOYSTICK_VERBOSE, ("Z Axis: %ld\n", js.lZ));
  IwTrace(JOYSTICK_VERBOSE, ("lRz: %ld\n", js.lRz));

  joystick.mAnalogueInputs[0] = js.lX;
  joystick.mAnalogueInputs[1] = js.lY;
  joystick.mAnalogueInputs[2] = js.lZ;
  joystick.mAnalogueInputs[3] = js.lRx;
  joystick.mAnalogueInputs[4] = js.lRy;
  joystick.mAnalogueInputs[5] = js.lRz;
  joystick.mAnalogueInputs[6] = js.rglSlider[0];
  joystick.mAnalogueInputs[7] = js.rglSlider[1];

  memcpy(joystick.mButtons, js.rgbButtons, sizeof(joystick.mButtons));
  memcpy(joystick.mPOVDirections, js.rgdwPOV, sizeof(js.rgdwPOV));

  s_joysticks[id]->deviceName(joystick.mName);

  return S3E_RESULT_SUCCESS;
}

//---------------------------------------------------------------------------------------------------------------------
void CalibrateJoystick_platform()
{
  IwTrace(JOYSTICK, ("Launching calibration"));
  LPSTR lpCommandLine = "control.exe joy.cpl";

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );

  CreateProcess(
    NULL,
    lpCommandLine,
    NULL, NULL,
    FALSE, 0, NULL, NULL,
    &si,
    &pi);
}
