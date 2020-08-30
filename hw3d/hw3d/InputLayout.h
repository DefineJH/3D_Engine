#pragma once
#include "Bindable.h"
#include "Vertex.h"

class InputLayout :
	public Bindable
{
public:
	InputLayout(Graphics& gfx,
		DynamicVertex::VertexLayout layout_in,
		ID3DBlob * pVertexShaderByteCode);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const DynamicVertex::VertexLayout& layout,
		ID3DBlob * pVertexShaderByteCode);
	static std::string GenerateUID(const DynamicVertex::VertexLayout& layout,
		ID3DBlob * pVertexShaderByteCode = nullptr);
	std::string GetUID() const noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	DynamicVertex::VertexLayout layout;
};


