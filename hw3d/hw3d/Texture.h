#pragma once
#include "Bindable.h"
#include "Surface.h"
class Texture :
	public Bindable
{
public:
	Texture(Graphics& gfx, const class Surface& s, unsigned int slot = 0);
	virtual void Bind(Graphics& gfx) noexcept;
private:
	unsigned int slot;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};

