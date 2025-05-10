#include "wnd.hpp"

Wnd::WndClass Wnd::WndClass::m_wndClass;
HINSTANCE Wnd::WndClass::m_hInst = ::GetModuleHandle( nullptr );

Wnd::Exception::Exception( int line, const char* file, std::string_view msg ) noexcept
	: m_line( line ), m_file( file ), m_msg( msg ), m_hr( E_FAIL )
{ }

Wnd::Exception::Exception( int line, const char* file, HRESULT hr ) noexcept
	: m_line( line ), m_file( file ), m_hr( hr )
{
	m_msg = translateErrorCode( hr );
}

Wnd::Exception::Exception( int line, const char* file, HRESULT hr, std::string_view msg ) noexcept
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

std::string Wnd::Exception::translateErrorCode( HRESULT hr ) const noexcept
{
	char* pMsgBuf{ nullptr };
	auto len = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>( &pMsgBuf ), 0UL, nullptr
	);
	
	std::string errorStr{ ( len == 0UL ) ? "Unknown error code" : pMsgBuf };
	
	if ( pMsgBuf ) {
		LocalFree( pMsgBuf );
	}

	return errorStr;
}

Wnd::WndClass::WndClass() noexcept
{
	WNDCLASSEX wcx;
	::ZeroMemory( &wcx, sizeof( wcx ) );

	wcx.cbSize = sizeof( wcx );
	wcx.style = CS_OWNDC;
	wcx.lpfnWndProc = wndProcBridge;
	wcx.hInstance = getInstance();
	wcx.lpszClassName = getName();

	::RegisterClassEx( &wcx );
}

Wnd::WndClass::~WndClass() noexcept {
	::UnregisterClass( getName(), getInstance() );
}

Wnd::Wnd(int width, int height, const char* title)
	: m_width(width), m_height(height), m_title(title)
{
	initWindow();
	initD3D();
}

Wnd::~Wnd() noexcept 
{
	cleanupD3D();
	if ( m_hWnd ) {
		::DestroyWindow( m_hWnd );
	}
}

void Wnd::initWindow()
{
	WNDCLASSEX wcx;
	::ZeroMemory( &wcx, sizeof( wcx ) );
	wcx.cbSize = sizeof( wcx );
	wcx.style = CS_OWNDC;
	wcx.lpfnWndProc = wndProcBridge;
	wcx.hInstance = WndClass::getInstance();
	wcx.lpszClassName = WndClass::getName();
	::RegisterClassEx( &wcx );

	m_hWnd = ::CreateWindow(
		WndClass::getName(),
		m_title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_width, m_height,
		nullptr,
		nullptr,
		WndClass::getInstance(),
		this // Passes 'this' so WndProcBridge can access the class instance
	);

	if ( !m_hWnd ) {
		throw WND_EXCEPT_HR_MSG( GetLastError(), "hWnd is nullptr" );
	}

	ShowWindow( m_hWnd, SW_SHOWDEFAULT );
}

void Wnd::initD3D()
{
	// Configure the swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	::ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1U;
	sd.OutputWindow = m_hWnd;
	sd.Windowed = TRUE;

	UINT flags{ 0U };
#ifndef NDEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the Direct3D device and swap chain
	HRESULT hr{ D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0U,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwap,
		&m_pDevice,
		nullptr,
		&m_pContext
	) };
	if ( FAILED( hr ) ) {
		throw WND_EXCEPT_HR_MSG( hr, "D3D11CreateDeviceAndSwapChain failed" );
	}

	// Retrieve the backbuffer from the swap chain
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	hr = m_pSwap->GetBuffer( 0U, __uuidof( ID3D11Resource ), &pBackBuffer );
	if ( FAILED( hr ) ) {
		throw WND_EXCEPT_HR_MSG( hr, "GetBuffer failed" );
	}

	// Create the render target view for the backbuffer
	hr = m_pDevice->CreateRenderTargetView( pBackBuffer.Get(), nullptr, &m_pTarget );
	if ( FAILED( hr ) ) {
		throw WND_EXCEPT_HR_MSG( hr, "CreateRenderTargetView failed" );
	}
}

void Wnd::cleanupD3D() noexcept
{
	if ( m_pContext ) {
		m_pContext->ClearState();
	}
}

LRESULT __stdcall Wnd::wndProcBridge( HWND hWnd,
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
		return pWnd->wndProc( hWnd, Msg, wParam, lParam );
	}

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}

LRESULT __stdcall Wnd::wndProc( HWND hWnd, 
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

std::optional<int> Wnd::processMsgs() const noexcept
{
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Retrieve and remove messages from the queue
	while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) )
	{
		if ( msg.message == WM_QUIT ) {
			return static_cast<int>( msg.wParam );
		}
	
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	return {};
}

void Wnd::renderFrame()
{
	const float CLEAR_COLOR[4] = { .4f, .2f, 1.f, 1.f };

	// Clear the render target view with the specified color
	m_pContext->ClearRenderTargetView( m_pTarget.Get(), CLEAR_COLOR );
	
	// Present the rendered frame to the screen
	m_pSwap->Present( 1U, 0U );
}