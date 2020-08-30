#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indicies);
	IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indicies);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indicies);
	template<typename...Ignore>
	static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
	{
		return GenerateUID_(tag);
	}
	std::string GetUID() const noexcept override;
	UINT GetCount() const noexcept;
private:
	static std::string GenerateUID_(const std::string& tag);
private:
	std::string tag;
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};


