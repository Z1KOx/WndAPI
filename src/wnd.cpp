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

Wnd::Wnd( int width, int height, const char* title )
	: m_width( width ), m_height( height ), m_title( title )
{
	m_hWnd = ::CreateWindow(
		WndClass::getName(),
		title,
		WS_POPUP,
		250, 250,
		m_width, m_height,
		nullptr, nullptr,
		WndClass::getInstance(),
		this // Passes 'this' so WndProcBridge can access the class instance
	);

	if ( nullptr == m_hWnd ) {
		throw WND_EXCEPT_HR_MSG( GetLastError(), "hWnd is nullptr" );
	}

	::ShowWindow( m_hWnd, SW_SHOWDEFAULT );

	m_pD3D = std::make_unique<D3D>( m_hWnd, m_width, m_height );
}

Wnd::~Wnd() noexcept 
{
	if ( m_hWnd ) {
		::DestroyWindow( m_hWnd );
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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd,
                                                              UINT msg,
                                                              WPARAM wParam,
                                                              LPARAM lParam );

LRESULT __stdcall Wnd::wndProc( HWND hWnd, 
                                UINT Msg,
                                WPARAM wParam,
                                LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( hWnd, Msg, wParam, lParam ) ) { return true; }
	if ( false == m_isRunning ) {
		::PostQuitMessage(0);
		return 0;
	}

	switch ( Msg )
	{
	case WM_CLOSE:
	{
		::PostQuitMessage( 0 );
	} return 0;

	case WM_LBUTTONDOWN: {
		m_pos = MAKEPOINTS( lParam );
	} return 0;
	
	case WM_MOUSEMOVE: 
	{
		if ( wParam == MK_LBUTTON )
		{
			const auto points = MAKEPOINTS( lParam );
			auto rect = ::RECT{ };

			::GetWindowRect( m_hWnd, &rect );

			rect.left += ( points.x - m_pos.x );
			rect.top += ( points.y - m_pos.y );

			if ( m_pos.x >= 0 && m_pos.x <= m_width &&
				m_pos.y >= 0 && m_pos.y <= 19 )
			{
				::SetWindowPos(
					m_hWnd,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
			}
		}

	}return 0;
	}

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}

std::optional<int> Wnd::processMsgs() const noexcept
{
	MSG msg;
	::ZeroMemory( &msg, sizeof( msg ) );

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
	m_pD3D->beginFrame();

	m_pD3D->drawTriangle();

	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.f );
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
	ImGui::SetNextWindowPos( { 0.f, 0.f } );
	ImGui::SetNextWindowSize( { static_cast<float>( m_width ), static_cast<float>( m_height ) } );
	ImGui::Begin( m_title, &m_isRunning, ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoBackground |
	                                     ImGuiWindowFlags_NoSavedSettings |
	                                     ImGuiWindowFlags_NoCollapse );
	static bool s_Clicked = false;
	ImGui::TextColored( ImVec4( ImColor( 255, 0, 0, 255 ) ), "Button" );
	if ( ImGui::Button( "Click", { 55.f, 25.f } ) ) {
		s_Clicked = true;
	}

	if ( s_Clicked ) {
		ImGui::TextColored( ImVec4( ImColor( 0, 255, 0, 255 ) ), "Clicked" );
	}
	
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::End();
	m_pD3D->endFrame();
}