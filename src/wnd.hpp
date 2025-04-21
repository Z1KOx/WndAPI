#pragma once
#include "pch.h"

class Wnd
{
public:
	class Exception : public std::exception
	{
	public:
		Exception( int line, const char* file, const std::string& msg ) noexcept;
		Exception( int line, const char* file, HRESULT hr ) noexcept;
		Exception( int line, const char* file, HRESULT hr, const std::string& msg ) noexcept;

		~Exception() noexcept = default;
		Exception( const Exception& ) = delete;
		Exception& operator=( const Exception& ) = delete;
	
	public:
		[[nodiscard]] char const* what() const noexcept override;

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
		HRESULT m_hr;
		mutable std::string m_whatBuffer;
	};

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
public:
	Wnd( int width, int height, const char* title );
	~Wnd() noexcept;
	Wnd( const Wnd& ) = delete;
	Wnd& operator=( const Wnd& ) = delete;

public:
	static LRESULT __stdcall WndProcBridge( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT __stdcall WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

private:
	int m_width = 0, m_height = 0;
	HWND m_hWnd = nullptr;
};

#define WND_EXCEPT_HR( hr )            Wnd::Exception( __LINE__, __FILE__, hr )
#define WND_EXCEPT_MSG( msg )          Wnd::Exception( __LINE__, __FILE__, msg )
#define WND_EXCEPT_HR_MSG( hr, msg )   Wnd::Exception( __LINE__, __FILE__, hr, msg )
