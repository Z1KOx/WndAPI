#pragma once
#include "bindable.hpp"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer( ID3D11Device* pDevice, const std::vector<unsigned short>& indices );
	void bind( ID3D11DeviceContext* pContext ) const override;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
};