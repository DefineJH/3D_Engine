#include "IndexBuffer.h"
#include "GraphicsThrowMacro.h"

IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indicies)
	:
	count( (UINT)indicies.size())
{
	INFOMAN(gfx);
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = UINT(count * sizeof(unsigned short));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = indicies.data();

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pIndexBuffer));

}

void IndexBuffer::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return count;
}
