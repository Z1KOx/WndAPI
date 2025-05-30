#pragma once
#include "pch.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "bindable/mesh.hpp"

class D3D
{
public:
    D3D() noexcept = default;
    D3D( HWND hWnd, int width, int height ) noexcept;
    ~D3D() noexcept;

    D3D( const D3D& ) = delete;
    D3D& operator=( const D3D& ) = delete;

public:
    void beginFrame() const noexcept;
    void endFrame() const noexcept;
    void drawTriangle() noexcept;

private:
    void setupImGui() const noexcept;

private:
    HWND m_hWnd = nullptr;
    std::unique_ptr<Mesh> m_mesh;
    Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwap;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTarget;
};