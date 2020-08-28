#include "VertexShader.h"
#include "GraphicsThrowMacro.h"
#include "BindableCodex.h"
#include <typeinfo>
#include <d3dcompiler.h>

namespace Bind
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path)
		:
		path(path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &pBytecodeBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader));
	}

	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

	std::shared_ptr<Bind::Bindable> VertexShader::Resolve(Graphics& gfx, const std::string& path)
	{
		//Bindable이 이미 있는지 확인
		std::shared_ptr<Bindable> bind = Codex::Resolve<VertexShader>(gfx, path);
		return bind;
	}

	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}

	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}

	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

}