#include "TransformCbuf_Double.h"

TransformCbuf_Double::TransformCbuf_Double(Graphics& gfx, const Drawable& parent, UINT vertex_slot, UINT pixel_slot)
	:
	TransformCbuf(gfx, parent, vertex_slot)
{
	if (!pPCBuf)
	{
		pPCBuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, pixel_slot);
	}
}

void TransformCbuf_Double::Bind(Graphics& gfx) noexcept
{
	const auto tr = GetTransforms(gfx);
	TransformCbuf::UpdateBindImpl(gfx, tr);
	UpdateBindImpl(gfx, tr);
}

void TransformCbuf_Double::UpdateBindImpl(Graphics& gfx, const Transforms& tr) noexcept
{
	pPCBuf->Update(gfx, tr);
	pPCBuf->Bind(gfx);
}

std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCbuf_Double::pPCBuf;