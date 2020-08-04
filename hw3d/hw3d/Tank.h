#pragma once
#include "TestObj.h"
class Tank :
	public TestObj<Tank>
{
public:
	Tank(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 materialColor);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
protected:
	DirectX::XMFLOAT3X3 model;
};

