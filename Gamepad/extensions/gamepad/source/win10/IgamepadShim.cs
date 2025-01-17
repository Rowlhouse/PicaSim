/*
C# implementation of the gamepad extension.

Add win8-specific functionality here.

Class for this Interface will be implement in native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
namespace gamepadManaged
{
    public interface IgamepadShim
    {
        bool Init(gamepadManaged managedAPI);
        bool Terminate();
    }
}
