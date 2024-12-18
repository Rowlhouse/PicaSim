/*
C++ Shim implementation of the gamepad extension's.

Not intended for modifications

These functions are called by C# to access native interface and from C++ to
access managed interface.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "gamepad_shim.h"

using namespace gamepadExtension;


gamepadManaged::gamepadManaged^ gamepadShim::s_ManagedInterface = nullptr;

gamepadShim::gamepadShim()
{
}

gamepadShim::~gamepadShim()
{
    Terminate();
}

bool gamepadShim::Init(gamepadManaged::gamepadManaged^ managedAPI)
{
    // It's an error to do this twice.
    if (s_ManagedInterface)
        return false;

    s_ManagedInterface = managedAPI;
    return true;
}

bool gamepadShim::Terminate()
{
    // It's an error to do this twice.
    if (!s_ManagedInterface)
        return false;

    s_ManagedInterface = nullptr;
    return true;
}

gamepadManaged::gamepadManaged^ gamepadShim::GetInterface()
{
    return s_ManagedInterface;
}
