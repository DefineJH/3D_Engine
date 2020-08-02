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
	//transform constant buffer에게 넘겨주기 위한 오브젝트의 월드공간의 좌표
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnBoxControllWindow(Graphics& gfx);
	void UpdateMaterial(Graphics& gfx);
protected:
	// transform 기본적용 ( 스케일 , 회전 )
	DirectX::XMFLOAT3X3 mt;

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularInten = 0.6f;
		float specularPower = 30.0f;
		float padd[3];
	};

	PSMaterialConstant matConst;
	
};

