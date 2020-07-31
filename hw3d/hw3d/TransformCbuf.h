#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>
class TransformCbuf :
	public Bindable
{
public:
	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
	TransformCbuf(Graphics& gfx, const Drawable& parent , UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;
private:
	//보면 constbuffer로 가지고 있다. 이는 즉, 매 프레임마다 실제 버텍스 버퍼를 바꾸는 것이 아닌
	//버텍스 쉐이더에 제공하는 상수버퍼의 값으로 연산을 진행해 화면에 출력하는 것임이 중요하다
	//그리고 constantbuffer도 객체마다 가질 필요가없다, 사실 bind 가 불릴때 vcbuf를 업데이트하여 파이프라인에 제출하는 것이기 때문에
	//bind가 불릴때 parent의 월드좌표를 가져오지만 이를 연산하여 넣는 constbuffer는 동일하다, 즉 메모리를 아낄 수 있다.
	static std::unique_ptr<VertexConstantBuffer<Transforms>> vcbuf;
	// 나중에 현재 transfromcbuf가 속해있는 객체의 월드 좌표행렬을 알아야 버텍스 버퍼에 연산을 진행가능하기 때문에
	const Drawable& parent;
};

