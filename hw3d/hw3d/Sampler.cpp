#include "Sampler.h"
#include "GraphicsThrowMacro.h"
#include "BindableCodex.h"

Sampler::Sampler(Graphics & gfx)
{
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sd, &pSampler));
}

void Sampler::Bind(Graphics & gfx) noexcept
{
	GetContext(gfx)->PSSetSamplers(0, 1, pSampler.GetAddressOf());
}

std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
{
	return Codex::Resolve<Sampler>(gfx);
}

std::string Sampler::GenerateUID()
{
	return typeid(Sampler).name();
}

std::string Sampler::GetUID() const noexcept
{
	return GenerateUID();
}

