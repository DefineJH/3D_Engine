#pragma once
#include "TransformCbuf.h"
class TransformCbuf_Double :
	public TransformCbuf
{
public:
	TransformCbuf_Double(Graphics& gfx, const Drawable& parent, UINT vertex_slot = 0u , UINT pixel_slot = 0u);
	void Bind(Graphics& gfx) noexcept override;
protected:
	void UpdateBindImpl(Graphics& gfx, const Transforms& tr) noexcept;
private:
	static std::unique_ptr<PixelConstantBuffer<Transforms>> pPCBuf;
};

