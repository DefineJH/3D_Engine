#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacro.h"
#include "Cube.h"

Box::Box(Graphics& gfx, std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 materialColor
	)
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

	//box는 TransCBuf즉, 객체의 위치를 제외하고는 모든 bindable
	// index buffer , vertex buffer , input layout , vertex shader, pixelshader, topology , pixelconstant를 공유할 수 있다
	if (!IsStaticInitalized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto cube = Cube::MakeIndependent<Vertex>();
		cube.SetNormalsIndependentFlat();

		cube.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		//버텍스버퍼 바인딩
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, cube.vertices));

		//인덱스 버퍼 바인딩
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, cube.indices));



		//버텍스 쉐이더바인딩
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		//나중 inputlayout을 위해 받아둔다
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		//픽셀 쉐이더 바인딩
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		//토폴로지 바인딩 , 기본 TriangleList
		AddStaticBind(std::make_unique<Topology>(gfx));

		//인풋 레이아웃 바인딩
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	}
	else
	{
		SetIndexFromStatic();
	}
	//transform버퍼 바인딩
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	struct PSMaterialConstant
	{
		alignas(16) DirectX::XMFLOAT3 color;
		float specularInten = 0.6f;
		float specularPower = 30.0f;
		float padd[2];
	} colorConst;

	colorConst.color = materialColor;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);

}

void Box::Update(float dt) noexcept
{
	xRot += dxRot * dt;
	yRot += dyRot * dt;
	zRot += dzRot * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return
		DirectX::XMLoadFloat3x3(&mt) *
		DirectX::XMMatrixRotationRollPitchYaw(yRot, zRot, xRot) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
