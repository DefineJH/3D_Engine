#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"
class VertexBuffer :
	public Bindable
{
public:
	//���ؽ��� �츮�� ���� �����ϴ� Ŭ���� Ȥ�� ����ü�̱� ������ ���ø�ȭ�Ͽ� ���ؽ� ������ �ٲ�������� �����۵��ϰ� �Ѵ�
	VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf);
	void Bind(Graphics& gfx) noexcept override;
protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};


