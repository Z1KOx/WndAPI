#include "index_buffer.hpp"

IndexBuffer::IndexBuffer( ID3D11Device* pDevice, const std::vector<unsigned short>& indices )
{
	D3D11_BUFFER_DESC bd;
	::ZeroMemory( &bd, sizeof( bd ) );
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>( indices.size() * sizeof( unsigned short ) );
	bd.StructureByteStride = sizeof( unsigned short );

	D3D11_SUBRESOURCE_DATA sd;
	::ZeroMemory( &sd, sizeof( sd ) );
	sd.pSysMem = indices.data();

	HRESULT hr = pDevice->CreateBuffer( &bd, &sd, &m_pIndexBuffer );
	if ( FAILED( hr ) ) {
		throw std::runtime_error( "Failed to create index buffer" );
	}
}

void IndexBuffer::bind( ID3D11DeviceContext* pContext ) const
{
	pContext->IASetIndexBuffer( m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0U );
}