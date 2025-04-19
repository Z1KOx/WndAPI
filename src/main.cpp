#include "wnd.hpp"

int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdLine)
{
	Wnd wnd( 800, 600, "Title" );

	MSG msg;
	while ( GetMessage( &msg, nullptr, 0, 0 ) > 0 ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}