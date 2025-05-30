#include "d3d.hpp"
#include "bindable/mesh.hpp"
#include "bindable/vertex_buffer.hpp"

using namespace Microsoft::WRL;

D3D::D3D( HWND hWnd, int width, int height ) noexcept
    : m_hWnd( hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ::ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1U;
    sd.OutputWindow = m_hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0U;
#ifndef NDEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, nullptr, 0U, D3D11_SDK_VERSION,
        &sd, &m_pSwap, &m_pDevice, nullptr, &m_pContext
    );
    if ( FAILED( hr ) ) {
        throw std::runtime_error( "Failed to create device and swap chain" );
    }

    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    m_pSwap->GetBuffer( 0, __uuidof( ID3D11Resource ), &pBackBuffer );
    m_pDevice->CreateRenderTargetView( pBackBuffer.Get(), nullptr, &m_pTarget );

    setupImGui();
}

D3D::~D3D() noexcept
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void D3D::beginFrame() const noexcept
{
    float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    m_pContext->ClearRenderTargetView( m_pTarget.Get(), clearColor );

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    m_pContext->OMSetRenderTargets( 1U, m_pTarget.GetAddressOf(), nullptr );
}

void D3D::endFrame() const noexcept
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
    m_pSwap->Present( 1U, 0U );
}

void D3D::drawTriangle() noexcept
{
    std::vector<Vertex> vertices = {
        { 0.0f, 0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        {-0.5f, -0.5f, 0.0f }
    };
    std::vector<unsigned short> indices = { 0, 1, 2 };

    std::wstring vsPath = L"shader\\vertexShader.cso";
    std::wstring psPath = L"shader\\pixelShader.cso";

    Mesh mesh( m_pDevice.Get(), vertices, indices, vsPath, psPath );

    D3D11_VIEWPORT vp;
    ::ZeroMemory( &vp, sizeof( vp ) );
    vp.Width = 550.0f;
    vp.Height = 350.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_pContext->RSSetViewports( 1u, &vp );

    mesh.draw( m_pContext.Get() );
}

void D3D::setupImGui() const noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); ( void )io;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init( m_hWnd );
    ImGui_ImplDX11_Init( m_pDevice.Get(), m_pContext.Get() );
}