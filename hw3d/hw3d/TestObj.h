#pragma once
#include "DrawableBase.h"


template<typename T>
class TestObj : public DrawableBase<T>
{
public:
	TestObj(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist)
		:
		r(rdist(rng)),
		dxRot(ddist(rng)),
		dyRot(ddist(rng)),
		dzRot(ddist(rng)),
		dphi(odist(rng)),
		dtheta(odist(rng)),
		dchi(odist(rng)),
		chi(adist(rng)),
		theta(adist(rng)),
		phi(adist(rng))
	{

	}
	void Update(float dt) noexcept
	{
		xRot += dxRot * dt;
		yRot += dyRot * dt;
		zRot += dzRot * dt;
		theta += dtheta * dt;
		phi += dphi * dt;
		chi += dchi * dt;
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		namespace dx = DirectX;
		return dx::XMMatrixRotationRollPitchYaw(yRot, xRot, zRot) *
			dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
protected:
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
};