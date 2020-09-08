#pragma once
#include "Bindable.h"
#include "Surface.h"

class Texture :
	public Bindable
{
public:
	Texture(Graphics& gfx, const std::string& path, unsigned int slot = 0);
	virtual void Bind(Graphics& gfx) noexcept;
	static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, unsigned int slot = 0);
	static std::string GenerateUID(const std::string& path, unsigned int slot);
	std::string GetUID() const noexcept override;
	bool HasAlpha() const noexcept;
private:
	unsigned int slot;
protected:
	bool hasAlpha = false; 
	std::string path;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};


