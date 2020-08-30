#include "VertexBuffer.h"
#include "BindableCodex.h"


VertexBuffer::VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf)
	: VertexBuffer(gfx, "?", vbuf)
{}

VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag, const DynamicVertex::VertexBuffer vbuf)
	:
	tag(tag),
	vbuf(std::move(vbuf)),
	stride((UINT)vbuf.GetLayout().Size())
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.ByteWidth = UINT(vbuf.SizeBytes());
	bd.MiscFlags = 0u;
	bd.StructureByteStride = stride;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vbuf.GetData();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
}

	

void VertexBuffer::Bind(Graphics& gfx) noexcept
{
	const UINT offset = 0u;
	GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
}



std::shared_ptr<Bindable> VertexBuffer::Resolve(Graphics& gfx,const std::string& tag, const DynamicVertex::VertexBuffer& vbuf)
{
	return Codex::Resolve<VertexBuffer>(gfx,tag ,vbuf);
}

std::string VertexBuffer::GetUID() const noexcept
{
	return GenerateUID(tag);
}

std::string VertexBuffer::GenerateUID_(const std::string& tag)
{
	using namespace std::string_literals;
	return typeid(VertexBuffer).name() + "#"s + tag;
}
