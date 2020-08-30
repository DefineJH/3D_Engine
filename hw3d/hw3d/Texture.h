#pragma once
#include "Bindable.h"
#include "Surface.h"

class Texture :
	public Bindable
{
public:
	Texture(Graphics& gfx, const std::string& path, unsigned int slot = 0);
	virtual void Bind(Graphics& gfx) noexcept;
	static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path, unsigned int slot = 0);
	static std::string GenerateUID(const std::string& path, unsigned int slot);
	std::string GetUID() const noexcept override;
private:
	unsigned int slot;
protected:
	std::string path;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};


