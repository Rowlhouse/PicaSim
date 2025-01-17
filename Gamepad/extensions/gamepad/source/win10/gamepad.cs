/*
C# implementation of the gamepad extension.

Add win8-specific functionality here.

These functions are called via Shim class from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using System.Windows;
        

using s3e_native;
using Windows.UI.Popups;
using Windows.Foundation;

namespace gamepadManaged
{
    public sealed class gamepadManaged: IManagedS3EEDKAPI
    {
        public gamepadManaged(IgamepadNative native, IgamepadShim shim)
        {
            m_Shim = shim;
            m_Native = native;
        }

        public bool ExtRegister(IS3EAPIManager apiManager, object mainPage)
        {
            try
            {
                // Keep a reference to the API Manager in order to call other
                // APIs.
                m_APIManager = apiManager;
                m_MainPage = mainPage as Page;
                m_MainPanel = VisualTreeHelper.GetChild(m_MainPage, 0) as Panel;

                // Add the managed API to the API Manager
                if (!m_APIManager.RegisterManagedAPI("gamepad", this))
                    throw new System.Exception("Can't register Managed API");

                // Add the native API to the API manager.  Note that we don't own the native
                // interface, the API Manager does.  We want the two notions of managed and
                // native interface to be separate as there may be cases where we only want
                // one not the other.  It's only a matter of convenience that we create both
                // APIs in this ctr
                
                if (!m_APIManager.RegisterNativeAPI("gamepad", m_Native))
                    throw new System.Exception("Can't register Native API");

                

                // Pass the managed interface down
                m_Shim.Init(this);
            }
            catch (System.Exception e)
            {
                m_APIManager = null;
                m_Shim = null;
                m_Native = null;
                MessageDialog aDialog = new Windows.UI.Popups.MessageDialog("Failed to register gamepad : " + e.Message);
                IAsyncOperation<IUICommand> AsyncOp = aDialog.ShowAsync();
                s3e_native.s3eExtensionsShim.GetInterface().RegistrationDone();
                return false;
            }
            s3e_native.s3eExtensionsShim.GetInterface().RegistrationDone();
            return true;
        }
        IS3EAPIManager m_APIManager = null;
        Page m_MainPage = null;
        // hint: add UI Elements as children of m_MainPanel
        Panel m_MainPanel = null;
        IgamepadShim m_Shim = null;
        IgamepadNative m_Native = null;

        // managed platform functionality


        public uint gamepadGetNumDevices_managed()
        {
            return 0;
        }

        public uint gamepadGetDeviceId_managed(uint index)
        {
            return 0;
        }

        public uint gamepadGetNumAxes_managed(uint index)
        {
            return 0;
        }

        public uint gamepadGetNumButtons_managed(uint index)
        {
            return 0;
        }

        public uint gamepadGetButtons_managed(uint index)
        {
            return 0;
        }

        public int gamepadGetAxis_managed(uint index, uint axisIndex)
        {
            return 0;
        }

        public uint gamepadIsPointOfViewAvailable_managed(uint index)
        {
            return 0;
        }

        public int gamepadGetPointOfViewAngle_managed(uint index)
        {
            return 0;
        }

        public void gamepadUpdate_managed()
        {
            
        }

        public void gamepadReset_managed()
        {
            
        }

        public void gamepadCalibrate_managed()
        {
            
        }
    }
}
