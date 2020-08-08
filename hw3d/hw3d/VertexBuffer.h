#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"
class VertexBuffer :
	public Bindable
{
public:
	//���ؽ��� �츮�� ���� �����ϴ� Ŭ���� Ȥ�� ����ü�̱� ������ ���ø�ȭ�Ͽ� ���ؽ� ������ �ٲ�������� �����۵��ϰ� �Ѵ�
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

