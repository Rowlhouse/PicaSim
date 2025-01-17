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

namespace gamepadExtension
{
// This is the class which managed code can use to talk to native code.
[Windows::Foundation::Metadata::WebHostHidden]
public ref class gamepadNative sealed : public gamepadManaged::IgamepadNative
{
    // We shouldn't populate these defacto.  We should only add functions here as and when
    // necessary.
public:
    gamepadNative() {}
};


[Windows::Foundation::Metadata::WebHostHidden]
public ref class gamepadShim sealed : public gamepadManaged::IgamepadShim
{
public:

    gamepadShim();

    // We use two stage construction to avoid the use of exceptions.  We
    // don't want to ever register an API more than once (as it's a static
    // below).  If we did this initialization in the ctr then we'd either
    // have to fail silently or throw an exception.
    virtual bool Init(gamepadManaged::gamepadManaged^ managedAPI);

    // Unregisters the static interface.
    virtual bool Terminate();

    // Need static getters as we can't have public members in the class.
    static gamepadManaged::gamepadManaged^ GetInterface();
private:

    ~gamepadShim();

    // This is the unavoidable point where we need a static.  There's
    // no choice as we're converting to a C-API.
    static gamepadManaged::gamepadManaged^ s_ManagedInterface;
};

// char* <-> Platform::String^ wrappers
static inline Platform::String^ UTF8ToString(const char* utf8str)
{
    return s3e_native::s3eAPIDataTypeWrapper::CStringToPlatformString(reinterpret_cast<unsigned int>(utf8str));
}

static inline void StringToUTF8(char* utf8str, size_t size, Platform::String^ str)
{
    s3e_native::s3eAPIDataTypeWrapper::PlatformStringToCString(reinterpret_cast<unsigned int>(utf8str), size, str);
}

}
