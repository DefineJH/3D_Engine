#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "SolidSphere.h"
class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx , DirectX::FXMMATRIX view) const noexcept;
private:
	struct PointLightCBuf
	{
	public:
		//gpu는 데이터를 집어올때 16byte씩 집어간다, 고로 align
		alignas(16) DirectX::XMFLOAT3 pos; //16
		alignas(16) DirectX::XMFLOAT3 AmbientLight;
		alignas(16) DirectX::XMFLOAT3 DiffuseColor;
		float DiffuseInten;
		float AttenuationCon;
		float AttenuationLin;
		float AttenuationQuad;
	};
private:
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};

