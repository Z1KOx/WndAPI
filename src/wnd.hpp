#pragma once
#include "pch.h"
#include "d3d.hpp"

class Wnd
{
public:
	// Custom exception class for handling errors in the window
	class Exception : public std::exception
	{
	public:
		Exception( int line, const char* file, std::string_view msg ) noexcept;
		Exception( int line, const char* file, HRESULT hr ) noexcept;
		Exception( int line, const char* file, HRESULT hr, std::string_view msg ) noexcept;
		~Exception() noexcept = default;

		Exception( const Exception& ) = default;
		Exception( Exception&& ) noexcept = default;
		Exception& operator=( const Exception& ) = default;
		Exception& operator=( Exception&& ) noexcept = default;
	
	public:
		char const* what() const noexcept override;

	private:
		[[nodiscard]] inline int getLine() const noexcept { return m_line; }
		[[nodiscard]] inline std::string getFile() const noexcept { return m_file; }
		[[nodiscard]] inline std::string getMsg() const noexcept { return m_msg; }
		[[nodiscard]] inline HRESULT getErrorCode() const noexcept { return m_hr; }
		[[nodiscard]] inline std::string translateErrorCode( HRESULT hr ) const noexcept;
	
	private:
		mutable std::string m_whatBuffer; // 'mutable' So we can override the 'what()' virtual function
		const char*         m_file = nullptr;
		std::string         m_msg;
		HRESULT             m_hr = 0L;
		int                 m_line = 0;
	};

private:
	// Singleton to ensure only one window class is created
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
		static WndClass              m_wndClass;
		static HINSTANCE             m_hInst;
	};

public:
	Wnd( int width, int height, const char* title );
	~Wnd() noexcept;
	
	Wnd( const Wnd& ) = delete;
	Wnd& operator=( const Wnd& ) = delete;

public:
	[[nodiscard]] HWND getHWND() const noexcept { return m_hWnd; }
	[[nodiscard]] bool getRunningState() const noexcept { return m_isRunning; }

public:
	[[nodiscard]] std::optional<int> processMsgs() const noexcept;
	void renderFrame();

private:
	static LRESULT __stdcall wndProcBridge( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT __stdcall wndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

private:
	std::unique_ptr<D3D> m_pD3D;
	const char*          m_title = nullptr;
	POINTS               m_pos = { 0 };
	HWND                 m_hWnd = nullptr;
	bool                 m_isRunning = true;
	int                  m_width = 0;
	int                  m_height = 0;
};

// Macros for creating exceptions with error codes or custom messages
#define WND_EXCEPT_HR( hr )            Wnd::Exception( __LINE__, __FILE__, hr )
#define WND_EXCEPT_MSG( msg )          Wnd::Exception( __LINE__, __FILE__, msg )
#define WND_EXCEPT_HR_MSG( hr, msg )   Wnd::Exception( __LINE__, __FILE__, hr, msg )