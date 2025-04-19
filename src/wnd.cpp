#include "wnd.hpp"

Wnd::WndClass Wnd::WndClass::m_wndClass;
HINSTANCE Wnd::WndClass::m_hInst = ::GetModuleHandle( nullptr );

Wnd::WndClass::WndClass() noexcept
{
	WNDCLASSEX wcx;
	::ZeroMemory( &wcx, sizeof( wcx ) );

	wcx.cbSize = sizeof( wcx );
	wcx.style = CS_OWNDC;
	wcx.lpfnWndProc = WndProcBridge;
	wcx.hInstance = getInstance();
	wcx.lpszClassName = getName();

	::RegisterClassEx( &wcx );
}

Wnd::WndClass::~WndClass() noexcept {
	::UnregisterClass( getName(), getInstance() );
}

Wnd::Wnd( int width, int height, const char* title ) noexcept
	: m_width( width ), m_height( height )
{
	m_hWnd = ::CreateWindow(
		WndClass::getName(),
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_width, m_height,
		nullptr, nullptr,
		WndClass::getInstance(),
		this
	);

	if ( nullptr == m_hWnd ) {
		::MessageBox( nullptr, "Failed to create window", "Error", MB_ICONERROR );
		std::exit( EXIT_FAILURE );
	}

	::SetWindowLongPtr( m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );
	::ShowWindow( m_hWnd, SW_SHOWDEFAULT );
}

Wnd::~Wnd() noexcept {
	::DestroyWindow( m_hWnd );
}

LRESULT __stdcall Wnd::WndProcBridge( HWND hWnd,
                                      UINT Msg,
                                      WPARAM wParam,
                                      LPARAM lParam )
{
	Wnd* pWnd;
	::ZeroMemory( &pWnd, sizeof( pWnd ) );

	if ( Msg == WM_NCCREATE )
	{
		auto* pCreate = reinterpret_cast<CREATESTRUCT*>( lParam );
		pWnd = reinterpret_cast<Wnd*>( pCreate->lpCreateParams );
		::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWnd ) );
	}
	else {
		pWnd = reinterpret_cast<Wnd*>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	}

	if ( pWnd ) {
		return pWnd->WndProc( hWnd, Msg, wParam, lParam );
	}

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}

LRESULT __stdcall Wnd::WndProc( HWND hWnd, 
                                UINT Msg,
                                WPARAM wParam,
                                LPARAM lParam )
{
	if ( Msg == WM_CLOSE )
	{
		::PostQuitMessage( 0 );
		return 0;
	}

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}