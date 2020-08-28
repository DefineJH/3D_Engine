#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"
class VertexBuffer :
	public Bindable
{
public:
	//버텍스는 우리가 직접 정의하는 클래스 혹은 구조체이기 때문에 템플릿화하여 버텍스 정보가 바뀌었을때도 정상작동하게 한다
	VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf);
	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};


