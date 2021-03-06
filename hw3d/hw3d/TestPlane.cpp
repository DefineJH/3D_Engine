#include "TestPlane.h"
#include "Plane.h"
#include "Bindablebase.h"

TestPlane::TestPlane(Graphics& gfx, float size)
{
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	AddBind(Sampler::Resolve(gfx));

	AddBind(Texture::Resolve(gfx, "Images\\brickwall.jpg"));
	AddBind(Texture::Resolve(gfx, "Images\\brickwall_spec.jpg",1u));
	AddBind(Texture::Resolve(gfx, "Images\\brickwall_normal.jpg",2u));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPS_SpecNormal.cso"));

	//struct PSMaterialConstant
	//{
	//	float specularIntensity = 0.1f;
	//	float specularPower = 20.0f;
	//	float padding[2];
	//} pmc;
	//AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf_Double>(gfx, *this,0u,1u));
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
