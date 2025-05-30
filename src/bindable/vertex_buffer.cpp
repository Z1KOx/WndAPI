#include "vertex_buffer.hpp"

VertexBuffer::VertexBuffer( ID3D11Device* pDevice, const std::vector<Vertex>& vertices )
{
	D3D11_BUFFER_DESC bd;
	::ZeroMemory( &bd, sizeof( bd ) );
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>( vertices.size() * sizeof( Vertex ) );
	bd.StructureByteStride = sizeof( Vertex );

	D3D11_SUBRESOURCE_DATA sd;
	::ZeroMemory( &sd, sizeof( sd ) );
	sd.pSysMem = vertices.data();

	HRESULT hr = pDevice->CreateBuffer( &bd, &sd, &m_pVertexBuffer );
	if ( FAILED( hr ) ) {
		throw std::runtime_error( "Failed to create vertex buffer" );
	}
}

void VertexBuffer::bind( ID3D11DeviceContext* pContext ) const
{
	const UINT STRIDE = sizeof( Vertex );
	const UINT OFFSET = 0U;
	pContext->IASetVertexBuffers( 0U, 1U, m_pVertexBuffer.GetAddressOf(), &STRIDE, &OFFSET );
}
