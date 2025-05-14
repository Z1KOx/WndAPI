#include "wnd.hpp"

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdLine)
{
	try
	{
		Wnd wnd( 550, 350, "Our Title" );

		while ( wnd.getRunningState() )
		{
			if ( auto exitCode = wnd.processMsgs() ) {
				return *exitCode;
			}

			wnd.renderFrame();

			std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
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