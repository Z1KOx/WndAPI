#pragma once
#include "pch.h"

class Bindable
{
public:
    virtual ~Bindable() = default;
    virtual void bind( ID3D11DeviceContext* pContext ) const = 0;
};

class Drawable
{
public:
    void addBindable( std::unique_ptr<Bindable> bindable ) {
        m_bindable.emplace_back( std::move( bindable ) );
    }

    void draw( ID3D11DeviceContext* pContext ) const
    {
        for ( const auto& bindable : m_bindable ) {
            bindable->bind( pContext );
        }

        pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        pContext->DrawIndexed( m_indexCount, 0U, 0U );
    }

protected:
    std::vector<std::unique_ptr<Bindable>> m_bindable;
    UINT m_indexCount = 0u;
};