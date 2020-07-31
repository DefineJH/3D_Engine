#pragma once
#include "DrawableBase.h"
class Box :
	public DrawableBase<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 materialColor);
	void Update(float dt) noexcept override;
	//transform constant buffer에게 넘겨주기 위한 오브젝트의 월드공간의 좌표
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	float r;
	//Roll
	float xRot = 0.0f;
	//pitch
	float yRot = 0.0f;
	//Yaw
	float zRot = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float dxRot;
	float dyRot;
	float dzRot;
	float dtheta;
	float dphi;
	float dchi;
public:
	// transform 기본적용 ( 스케일 , 회전 )
	DirectX::XMFLOAT3X3 mt;
};

