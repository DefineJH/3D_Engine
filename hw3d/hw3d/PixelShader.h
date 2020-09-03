#pragma once
#include "Bindable.h"

class PixelShader :
	public Bindable
{
public:
	PixelShader(Graphics& gfx, const std::string& path);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<PixelShader> Resolve(Graphics& gfx, const std::string& path);
	static std::string GenerateUID(const std::string& path);
	std::string GetUID() const noexcept override;
private:
	std::string path;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};

