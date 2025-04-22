#include "wnd.hpp"

Wnd::WndClass Wnd::WndClass::m_wndClass;
HINSTANCE Wnd::WndClass::m_hInst = ::GetModuleHandle( nullptr );

Wnd::Exception::Exception( int line, const char* file, const std::string& msg ) noexcept
	: m_line( line ), m_file( file ), m_msg( msg ), m_hr( E_FAIL )
{ }

Wnd::Exception::Exception( int line, const char* file, HRESULT hr ) noexcept
	: m_line( line ), m_file( file ), m_hr( hr )
{
	m_msg = translateErrorCode( hr );
}

Wnd::Exception::Exception( int line, const char* file, HRESULT hr, const std::string& msg ) noexcept
	: m_line( line ), m_file( file ), m_hr( hr ), m_msg( msg )
{
	m_msg += "\n[SYSTEM]: ";
	m_msg += translateErrorCode( hr );
}

char const* Wnd::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "Window Error\n"
		<< "[FILE]: " << m_file << '\n'
		<< "[LINE]: " << m_line << '\n'
		<< "[CUSTOM_MESSAGE]: " << m_msg;
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

std::string Wnd::Exception::translateErrorCode(HRESULT hr) const noexcept
{
	char* pMsgBuf{ nullptr };
	auto len = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>( &pMsgBuf ), 0UL, nullptr
	);
	
	std::string errorStr{ ( len == 0UL ) ? "Unknown error code" : pMsgBuf };
	LocalFree( pMsgBuf );
	return errorStr;
}

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

Wnd::Wnd( int width, int height, const char* title )
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
		this // Passes 'this' so WndProcBridge can access the class instance
	);
	
	if ( nullptr == m_hWnd ) {
		throw WND_EXCEPT_HR_MSG( GetLastError(), "hWnd is nullptr" );
	}

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

	// On WM_NCCREATE: extract 'this' pointer and store it in WinAPI userdata
	if ( Msg == WM_NCCREATE )
	{
		auto* pCreate = reinterpret_cast<CREATESTRUCT*>( lParam );
		pWnd = reinterpret_cast<Wnd*>( pCreate->lpCreateParams );
		::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWnd ) );
	}
	// On other messages: retrieve 'this' pointer from WinAPI userdata
	else {
		pWnd = reinterpret_cast<Wnd*>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	}

	// Forward message to the instance WndProc
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