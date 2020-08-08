#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"
class VertexBuffer :
	public Bindable
{
public:
	//버텍스는 우리가 직접 정의하는 클래스 혹은 구조체이기 때문에 템플릿화하여 버텍스 정보가 바뀌었을때도 정상작동하게 한다
	template<typename V>
	VertexBuffer(Graphics& gfx, const std::vector<V>& vertices);
	VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf);
	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

template<typename V>
VertexBuffer::VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
	:
	stride(sizeof(V))
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = UINT(sizeof(V) * vertices.size());
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = stride;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = vertices.data();

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
}

