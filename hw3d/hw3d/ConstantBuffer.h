#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"

//const ���۴� � Ÿ���̴����� ���� �����Ͽ� �ѱ� �� �����Ƿ� ���ø�ȭ�Ѵ�
template<typename T>
class ConstantBuffer :
	public Bindable
{
public:
	//constbuffer�� ������Ʈ, �� ������ �ٲ� ��
	void Update(Graphics& gfx, const T& consts)
	{
		//infomanager ȹ��
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		//context::map�� ���� ���ҽ�( pConstBuffer ) �� ���� �����͸� �������� GPU�� �������� �ź��Ѵ�
		//d3d11_map_write_discard -> ���� ���ҽ��� �����
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr) );
		memcpy(msr.pData, &consts, sizeof(consts));
		//unmap -> cpu������ ���� �����͸� �Ҵɽ�Ű�� gpu�� �������� ����Ѵ�
		GetContext(gfx)->Unmap(pConstBuffer.Get(), 0u);

	}
	//const buffer�� ���� ( �����ͱ��� ���� �־��� ���� )
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
	//������ ���� ������ �� ����
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


//���ؽ� ���̴��� �ѱ�� ���� ����, ���� �ٸ��� ����
template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
	//���ø� Ŭ������ ��ӹ޾�����, protected�� ��� ����, �Լ��� �������� ������δ� this-> �� �����ϴ� ��İ�
	// using�� ����� ��������� ����Ʈ�ϴ¹��� �ִ�.
	using ConstantBuffer<T>::pConstBuffer;
	using ConstantBuffer<T>::slot;
	using Bindable::GetContext;
public:
	// �θ� Ŭ���� ������ ��� ���
	using ConstantBuffer<T>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstBuffer.GetAddressOf());
	}
};

//�ȼ� ���̴��� �ѱ�� ���� ����, ���� �ٸ��� ����
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
