#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacro.h"
#include "ImGUI/imgui.h"
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
	TestObj(gfx,rng,adist,ddist,odist,rdist)
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


	matConst.color = materialColor;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, matConst, 1u));
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);

}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return
		DirectX::XMLoadFloat3x3(&mt) *
		DirectX::XMMatrixRotationRollPitchYaw(yRot, zRot, xRot) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}

void Box::SpawnBoxControllWindow(Graphics& gfx)
{
	bool dirty = false;
	if (ImGui::Begin("Box Properties"))
	{
		ImGui::Text("Material Properties");
		auto cd = ImGui::ColorEdit3("Color", &matConst.color.x);
		auto sid = ImGui::SliderFloat("Specular Intensity", &matConst.specularInten, 0.0f, 1.5f);
		auto spd = ImGui::SliderFloat("Specular Power", &matConst.specularPower, 0.0f, 50.0f, "%.1f");
		dirty = cd || sid || spd;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.1f, 50.0f);
		ImGui::SliderFloat("xRot", &xRot, -90.0f, 90.0f);
		ImGui::SliderFloat("yRot", &yRot, -90.0f, 90.0f);
		ImGui::SliderFloat("zRot", &zRot, -90.0f, 90.0f);

		ImGui::SliderFloat("theta", &theta, -90.0f, 90.0f);
		ImGui::SliderFloat("phi", &phi, -90.0f, 90.0f);
		ImGui::SliderFloat("chi", &chi, -90.0f, 90.0f);

	}
	ImGui::End();
	if (dirty)
		UpdateMaterial(gfx);
}

void Box::UpdateMaterial(Graphics& gfx)
{
	auto pixelconstbuffer = QueryBindable<PixelConstantBuffer<PSMaterialConstant>>();
	if (pixelconstbuffer)
	{
		pixelconstbuffer->Update(gfx, matConst);
	}
}


