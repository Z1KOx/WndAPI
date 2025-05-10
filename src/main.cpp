#include "wnd.hpp"

namespace wrl = Microsoft::WRL;

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdLine)
{
	try
	{
		Wnd wnd( 800, 600, "Our Title" );

		while (true)
		{
			if (auto exitCode = wnd.processMsgs()) {
				return *exitCode;
			}

			wnd.renderFrame();
		}
	}
	catch ( const Wnd::Exception& e ) {
		::MessageBox( nullptr, e.what(), "Custom Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( const std::exception& e ) {
		::MessageBox( nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( ... ) {
		::MessageBox( nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION );
	}

	return -1;
}