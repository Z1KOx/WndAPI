#pragma once
#include "bindable.hpp"
#include <d3dcompiler.h>

class Shader : public Bindable
{
public:
    Shader( ID3D11Device* pDevice,
            const std::wstring& vsPath,
            const std::wstring& psPath,
            const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
            UINT numElements );

    void bind( ID3D11DeviceContext* pContext ) const override;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
};