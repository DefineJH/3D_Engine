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

	//box�� TransCBuf��, ��ü�� ��ġ�� �����ϰ�� ��� bindable
	// index buffer , vertex buffer , input layout , vertex shader, pixelshader, topology , pixelconstant�� ������ �� �ִ�
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

		//���ؽ����� ���ε�
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, cube.vertices));

		//�ε��� ���� ���ε�
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, cube.indices));



		//���ؽ� ���̴����ε�
		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		//���� inputlayout�� ���� �޾Ƶд�
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		//�ȼ� ���̴� ���ε�
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		//�������� ���ε� , �⺻ TriangleList
		AddStaticBind(std::make_unique<Topology>(gfx));

		//��ǲ ���̾ƿ� ���ε�
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	}
	else
	{
		SetIndexFromStatic();
	}
	//transform���� ���ε�
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
