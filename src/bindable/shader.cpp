#include "shader.hpp"

Shader::Shader( ID3D11Device* pDevice,
                const std::wstring& vsPath,
                const std::wstring& psPath,
                const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
                UINT numElements )
{
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
    HRESULT hr;

    hr = D3DReadFileToBlob( vsPath.c_str(), &pBlob );
    if ( FAILED( hr ) ) {
        throw std::runtime_error( "Failed to load vertex shader" );
    }
    hr = pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader );
    if ( FAILED( hr ) ) {
        throw std::runtime_error( "Failed to create vertex shader" );
    }
    hr = pDevice->CreateInputLayout( inputLayoutDesc, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pInputLayout );
    if ( FAILED( hr ) ) {
        throw std::runtime_error( "Failed to create input layout" );
    }

    hr = D3DReadFileToBlob( psPath.c_str(), &pBlob );
    if ( FAILED(hr ) ) {
        throw std::runtime_error( "Failed to load pixel shader" );
    }
    hr = pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader );
    if ( FAILED( hr ) ) {
        throw std::runtime_error( "Failed to create pixel shader" );
    }
}

void Shader::bind( ID3D11DeviceContext* pContext ) const
{
    pContext->IASetInputLayout( m_pInputLayout.Get() );
    pContext->VSSetShader( m_pVertexShader.Get(), nullptr, 0U );
    pContext->PSSetShader( m_pPixelShader.Get(), nullptr, 0U );
}