#pragma once
#include "bindable.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"
#include "shader.hpp"

class Mesh : public Drawable
{
public:
    Mesh( ID3D11Device* pDevice,
          const std::vector<Vertex>& vertices,
          const std::vector<unsigned short>& indices,
          const std::wstring& vsPath,
          const std::wstring& psPath );
};