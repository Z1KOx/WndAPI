#pragma once
#include "pch.h"

class Wnd
{
private:
	class WndClass
	{
	private:
		WndClass() noexcept;
		~WndClass() noexcept;
		WndClass( const WndClass& ) = delete;
		WndClass& operator=( const WndClass& ) = delete;
	public:
		static const char* getName() noexcept { return m_wndClassName; }
		static HINSTANCE getInstance() noexcept { return m_hInst; }
	private:
		static constexpr const char* m_wndClassName = "ClassName";
		static WndClass m_wndClass;
		static HINSTANCE m_hInst;
	};
public:
	Wnd( int width, int height, const char* title ) noexcept;
	~Wnd() noexcept;
	Wnd( const Wnd& ) = delete;
	Wnd& operator=( const Wnd& ) = delete;
public:
	static LRESULT __stdcall WndProcBridge( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	LRESULT __stdcall WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
private:
	int m_width, m_height;
	HWND m_hWnd;
};