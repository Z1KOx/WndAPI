#include "mesh.hpp"

Mesh::Mesh( ID3D11Device* pDevice,
            const std::vector<Vertex>& vertices,
            const std::vector<unsigned short>& indices,
            const std::wstring& vsPath,
            const std::wstring& psPath )
{
    addBindable( std::make_unique<VertexBuffer>( pDevice, vertices ) );
    addBindable( std::make_unique<IndexBuffer>( pDevice, indices ) );

    const D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    addBindable( std::make_unique<Shader>( pDevice, vsPath, psPath, ied, static_cast<UINT>( std::size( ied ) ) ) );
    m_indexCount = static_cast<UINT>( indices.size() );
}