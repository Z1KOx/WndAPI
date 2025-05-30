#pragma once
#include "bindable.hpp"

struct Vertex {
	float x, y, z;
};

class VertexBuffer : public Bindable
{
public:
	VertexBuffer( ID3D11Device* pDevice, const std::vector<Vertex>& vertices );
	void bind( ID3D11DeviceContext* pContext ) const override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
};