#include "wnd.hpp"

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdLine)
{
	try
	{
		Wnd wnd( 800, 600, "Our Title" );

		MSG msg;
		::ZeroMemory( &msg, sizeof( msg ) );
		while ( GetMessage( &msg, nullptr, 0, 0 ) > 0 )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
		
		if ( msg.wParam ) {
			return static_cast<int>( msg.wParam );
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