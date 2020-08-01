#pragma once
#include "TestObj.h"
class Box :
	public TestObj<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 materialColor);
	//transform constant buffer���� �Ѱ��ֱ� ���� ������Ʈ�� ��������� ��ǥ
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
protected:
	// transform �⺻���� ( ������ , ȸ�� )
	DirectX::XMFLOAT3X3 mt;
};

