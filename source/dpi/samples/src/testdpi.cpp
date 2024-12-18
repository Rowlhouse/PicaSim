#include <s3e.h>
#include <sstream>
#include <iwgx.h>
#include <dpiInfo.h>


//-----------------------------------------------------------------------------
// Main global function
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	IwGxInit();
	IwResManagerInit();
	DPI::dpiInit();
	IwGxSetColClear(0x1f, 0x1f, 0xc0, 0x7f);

	{

		for (;;)
		{
			s3eDeviceYield(0);
			s3eKeyboardUpdate();

			bool result = true;
			if	(
				(result == false) ||
				(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
				(s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
				(s3eDeviceCheckQuitRequest()) 
				)
				break;


			//IwGxClear(IW_GX_DEPTH_BUFFER_F);
			IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

			int32 d = DPI::dpiGetScreenDPI();
			std::stringstream stream;
			stream << "DPI = " << d;
			std::string s = stream.str();

			IwGxPrintString(20,20,s.c_str());

			IwGxFlush();
			IwGxSwapBuffers();
		}
	}
	DPI::dpiTerminate();
	IwResManagerTerminate();
	IwGxTerminate();
}