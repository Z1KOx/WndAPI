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
		[[nodiscard]] int getLine() const noexcept { return m_line; }
		[[nodiscard]] std::string getFile() const noexcept { return m_file; }
		[[nodiscard]] std::string getMsg() const noexcept { return m_msg; }
		[[nodiscard]] HRESULT getErrorCode() const noexcept { return m_hr; }
		[[nodiscard]] std::string translateErrorCode( HRESULT hr ) const noexcept;
	
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

	// Static WndProc redirects messages to this class's WndProc via the 'this' pointer
public:
	static LRESULT __stdcall WndProcBridge( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT __stdcall WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

private:
	int m_width = 0, m_height = 0;
	HWND m_hWnd = nullptr;
};

// Macros for creating exceptions with error codes or custom messages
#define WND_EXCEPT_HR( hr )            Wnd::Exception( __LINE__, __FILE__, hr )
#define WND_EXCEPT_MSG( msg )          Wnd::Exception( __LINE__, __FILE__, msg )
#define WND_EXCEPT_HR_MSG( hr, msg )   Wnd::Exception( __LINE__, __FILE__, hr, msg )