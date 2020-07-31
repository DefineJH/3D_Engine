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
	//���� constbuffer�� ������ �ִ�. �̴� ��, �� �����Ӹ��� ���� ���ؽ� ���۸� �ٲٴ� ���� �ƴ�
	//���ؽ� ���̴��� �����ϴ� ��������� ������ ������ ������ ȭ�鿡 ����ϴ� ������ �߿��ϴ�
	//�׸��� constantbuffer�� ��ü���� ���� �ʿ䰡����, ��� bind �� �Ҹ��� vcbuf�� ������Ʈ�Ͽ� ���������ο� �����ϴ� ���̱� ������
	//bind�� �Ҹ��� parent�� ������ǥ�� ���������� �̸� �����Ͽ� �ִ� constbuffer�� �����ϴ�, �� �޸𸮸� �Ƴ� �� �ִ�.
	static std::unique_ptr<VertexConstantBuffer<Transforms>> vcbuf;
	// ���߿� ���� transfromcbuf�� �����ִ� ��ü�� ���� ��ǥ����� �˾ƾ� ���ؽ� ���ۿ� ������ ���డ���ϱ� ������
	const Drawable& parent;
};

