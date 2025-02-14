/*
Generic implementation of the gamepad extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


// #include "File.h"
#include "gamepad_internal.h"
#include "Entete.h"

#include <SDL2/SDL.h>
#include "gamepad_internal.h"

static SDL_GameController* gamepad_devices[16] = { nullptr };
static int gamepad_device_count = 0;

void gamepadReleaseDevices()
{
    for (int i = 0; i < gamepad_device_count; ++i)
    {
        if (gamepad_devices[i])
        {
            SDL_GameControllerClose(gamepad_devices[i]);
            gamepad_devices[i] = nullptr;
        }
    }
    gamepad_device_count = 0;
}

void gamepadCaptureDevices()
{
    gamepadReleaseDevices();
    
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        IwTrace(GAMEPAD_VERBOSE, ("SDL_InitSubSystem failed: %s", SDL_GetError()));
        return;
    }
    
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks && i < 16; ++i)
    {
        if (SDL_IsGameController(i))
        {
            gamepad_devices[gamepad_device_count] = SDL_GameControllerOpen(i);
            if (gamepad_devices[gamepad_device_count])
            {
                IwTrace(GAMEPAD_VERBOSE, ("Controller %d captured: %s", i, SDL_GameControllerName(gamepad_devices[gamepad_device_count])));
                ++gamepad_device_count;
            }
        }
    }
}

/*void gamepadUpdate()
{
    SDL_GameControllerUpdate();
}

int32_t gamepadGetAxis(uint32_t index, uint32_t axisIndex)
{
    if (index >= gamepad_device_count || !gamepad_devices[index])
        return 0;
    
    SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(axisIndex);
    return SDL_GameControllerGetAxis(gamepad_devices[index], axis);
}

uint32_t gamepadGetButtons(uint32_t index)
{
    if (index >= gamepad_device_count || !gamepad_devices[index])
        return 0;
    
    uint32_t buttons = 0;
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
    {
        if (SDL_GameControllerGetButton(gamepad_devices[index], static_cast<SDL_GameControllerButton>(i)))
        {
            buttons |= (1 << i);
        }
    }
    return buttons;
}*/

s3eResult gamepadInit()
{
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
    {
        IwTrace(GAMEPAD_VERBOSE, ("SDL_Init failed: %s", SDL_GetError()));
        return S3E_RESULT_ERROR;
    }
    gamepadCaptureDevices();
    return S3E_RESULT_SUCCESS;
}

void gamepadTerminate()
{
    gamepadReleaseDevices();
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}


/*
s3eResult gamepadInit()
{
    //Add any generic initialisation code here
    return gamepadInit_platform();
}

void gamepadTerminate()
{
    //Add any generic termination code here
    gamepadTerminate_platform();
}

uint32_t gamepadGetNumDevices()
{
	return gamepadGetNumDevices_platform();
}

uint32_t gamepadGetDeviceId(uint32_t index)
{
	return gamepadGetDeviceId_platform(index);
}

uint32_t gamepadGetNumAxes(uint32_t index)
{
	return gamepadGetNumAxes_platform(index);
}

uint32_t gamepadGetNumButtons(uint32_t index)
{
	return gamepadGetNumButtons_platform(index);
}

uint32_t gamepadGetButtons(uint32_t index)
{
	return gamepadGetButtons_platform(index);
}

int32_t gamepadGetAxis(uint32_t index, uint32_t axisIndex)
{
	return gamepadGetAxis_platform(index, axisIndex);
}

uint32_t gamepadIsPointOfViewAvailable(uint32_t index)
{
	return gamepadIsPointOfViewAvailable_platform(index);
}

int32_t gamepadGetPointOfViewAngle(uint32_t index)
{
	return gamepadGetPointOfViewAngle_platform(index);
}

void gamepadUpdate()
{
	gamepadUpdate_platform();
}

void gamepadReset()
{
	gamepadReset_platform();
}

void gamepadCalibrate()
{
	gamepadCalibrate_platform();
}*/
