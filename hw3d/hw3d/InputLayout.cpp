#include "InputLayout.h"
#include "GraphicsThrowMacro.h"
#include "BindableCodex.h"
#include "Vertex.h"


InputLayout::InputLayout(Graphics& gfx,
	DynamicVertex::VertexLayout layout_in,
	ID3DBlob * pVertexShaderByteCode)
	:
	layout(std::move(layout_in))
{
	INFOMAN(gfx);
	const auto d3dLayout = layout.GetD3DLayout();
	//inputlayoutÀº vertex½¦ÀÌ´õ¿¡ ¹­´Â´Ù
	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(d3dLayout.data(), (UINT)d3dLayout.size(),
		pVertexShaderByteCode->GetBufferPointer(),
		pVertexShaderByteCode->GetBufferSize(),
		&pInputLayout));
}

void InputLayout::Bind(Graphics & gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}

std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, const DynamicVertex::VertexLayout& layout, ID3DBlob * pVertexShaderByteCode)
{
	return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderByteCode);
}

std::string InputLayout::GenerateUID(const DynamicVertex::VertexLayout& layout, ID3DBlob * pVertexShaderByteCode)
{
	using namespace std::string_literals;
	return typeid(InputLayout).name() + "#"s + layout.GetLayoutCode();
}

std::string InputLayout::GetUID() const noexcept
{
	return GenerateUID(layout);
}


