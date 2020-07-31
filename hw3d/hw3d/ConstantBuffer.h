#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"

//const 버퍼는 어떤 타입이던간에 직접 정의하여 넘길 수 있으므로 템플릿화한다
template<typename T>
class ConstantBuffer :
	public Bindable
{
public:
	//constbuffer를 업데이트, 즉 내용을 바꿀 때
	void Update(Graphics& gfx, const T& consts)
	{
		//infomanager 획득
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		//context::map은 하위 리소스( pConstBuffer ) 에 대한 포인터를 가져오고 GPU의 엑세스를 거부한다
		//d3d11_map_write_discard -> 하위 리소스를 덮어쓴다
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr) );
		memcpy(msr.pData, &consts, sizeof(consts));
		//unmap -> cpu접근을 위한 포인터를 불능시키고 gpu의 엑세스를 허락한다
		GetContext(gfx)->Unmap(pConstBuffer.Get(), 0u);

	}
	//const buffer를 생성 ( 데이터까지 같이 넣어준 버전 )
	ConstantBuffer(Graphics& gfx, const T& consts , UINT slot = 0u)
		: slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstBuffer));
	}
	//데이터 없이 생성만 한 버전
	ConstantBuffer(Graphics& gfx , UINT slot = 0u)
		: slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(T);
		cbd.StructureByteStride = 0u;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstBuffer));
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstBuffer;
	UINT slot;
};


//버텍스 쉐이더에 넘기는 버전 생성, 딱히 다르지 않음
template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
	//템플릿 클래스를 상속받았을때, protected의 멤버 변수, 함수를 가져오는 방식으로는 this-> 로 접근하는 방식과
	// using을 사용해 명시적으로 임포트하는법이 있다.
	using ConstantBuffer<T>::pConstBuffer;
	using ConstantBuffer<T>::slot;
	using Bindable::GetContext;
public:
	// 부모 클래스 생성자 사용 명시
	using ConstantBuffer<T>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstBuffer.GetAddressOf());
	}
};

//픽셀 쉐이더에 넘기는 버전 생성, 딱히 다르지 않음
template<typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::pConstBuffer;
	using ConstantBuffer<T>::slot;
	using Bindable::GetContext;
public:
	using ConstantBuffer<T>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstBuffer.GetAddressOf());
	}
};
