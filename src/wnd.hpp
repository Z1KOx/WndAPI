#pragma once
#include "pch.h"

class Wnd
{
	// Custom exception class for handling errors in the window
public:
	class Exception : public std::exception
	{
	public:
		Exception( int line, const char* file, std::string_view msg ) noexcept;
		Exception( int line, const char* file, HRESULT hr ) noexcept;
		Exception( int line, const char* file, HRESULT hr, std::string_view msg ) noexcept;

		~Exception() noexcept = default;
		Exception( const Exception& ) = delete;
		Exception& operator=( const Exception& ) = delete;
	
	public:
		char const* what() const noexcept override;

	private:
		[[nodiscard]] inline int getLine() const noexcept { return m_line; }
		[[nodiscard]] inline std::string getFile() const noexcept { return m_file; }
		[[nodiscard]] inline std::string getMsg() const noexcept { return m_msg; }
		[[nodiscard]] inline HRESULT getErrorCode() const noexcept { return m_hr; }
		[[nodiscard]] inline std::string translateErrorCode( HRESULT hr ) const noexcept;
	
	private:
		int m_line = 0;
		const char* m_file = nullptr;
		std::string m_msg;
		HRESULT m_hr = 0L;
		mutable std::string m_whatBuffer; // 'mutable' So we can override the 'what()' virtual function
	};

	// Singleton to ensure only one window class is created
private:
	class WndClass
	{
	private:
		WndClass() noexcept;
		~WndClass() noexcept;
		WndClass( const WndClass& ) = delete;
		WndClass& operator=( const WndClass& ) = delete;

	public:
		[[nodiscard]] static const char* getName() noexcept { return m_wndClassName; }
		[[nodiscard]] static HINSTANCE getInstance() noexcept { return m_hInst; }

	private:
		static constexpr const char* m_wndClassName = "ClassName";
		static WndClass m_wndClass;
		static HINSTANCE m_hInst;
	};

	// Handles window creation, messages, and cleanup
public:
	Wnd( int width, int height, const char* title );
	~Wnd() noexcept;
	Wnd( const Wnd& ) = delete;
	Wnd& operator=( const Wnd& ) = delete;

public:
	[[nodiscard]] HWND getHWND() const noexcept { return m_hWnd; }

	// Window message handling and rendering
public:
	static LRESULT __stdcall wndProcBridge( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT __stdcall wndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	[[nodiscard]] std::optional<int> processMsgs() const noexcept;
	void renderFrame();

	// Window and Direct3D initialization and cleanup
private:
	void initWindow();
	void initD3D();
	void cleanupD3D() noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTarget;

	int m_width = 0, m_height = 0;
	const char* m_title = nullptr;
	HWND m_hWnd = nullptr;
};

// Macros for creating exceptions with error codes or custom messages
#define WND_EXCEPT_HR( hr )            Wnd::Exception( __LINE__, __FILE__, hr )
#define WND_EXCEPT_MSG( msg )          Wnd::Exception( __LINE__, __FILE__, msg )
#define WND_EXCEPT_HR_MSG( hr, msg )   Wnd::Exception( __LINE__, __FILE__, hr, msg )