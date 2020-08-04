#include "Tank.h"
#include "BindableBase.h"
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

Tank::Tank(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 materialColor)
	:
	TestObj(gfx,rng,adist,ddist,odist,rdist)
{
	if (!IsStaticInitalized())
	{
		Assimp::Importer imp;
		auto model = imp.ReadFile("Models//tank_fixed2.fbx",
			0u);
		if (!model || model->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !model->mRootNode)
		{
			return;
		}

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};
		std::vector<Vertex> verts;
		std::vector<unsigned short> indicies;
		
		for (UINT i = 0; i < model->mNumMeshes; i++)
		{
			for (UINT j = 0; j < model->mMeshes[i]->mNumVertices; j++)
			{
				verts.push_back({ { model->mMeshes[i]->mVertices[j].x ,model->mMeshes[i]->mVertices[j].y, model->mMeshes[i]->mVertices[j].z} ,
					*reinterpret_cast<DirectX::XMFLOAT3*>(&model->mMeshes[i]->mNormals[j]) });
			}
			for (UINT k = 0; k < model->mMeshes[i]->mNumFaces; k++)
			{
				const auto& face = model->mMeshes[i]->mFaces[k];
				assert(face.mNumIndices == 3);
				for (UINT j = 0; j < face.mNumIndices; j++)
					indicies.push_back(face.mIndices[j]);
			}
		}

		AddStaticBind(std::make_unique<Topology>(gfx));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, verts));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indicies));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto bc = pvs->GetBytecode();

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, bc));
		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 matCol;
		float specularInten = 0.6f;
		float specularPower = 30.0f;
		float padd[3];

	}matConst;
	matConst.matCol = materialColor;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, matConst, 1u));
	DirectX::XMStoreFloat3x3(
		&model,
		DirectX::XMMatrixScaling(0.5f,0.5f,0.5f)
	);
}

DirectX::XMMATRIX Tank::GetTransformXM() const noexcept
{
	return
		DirectX::XMLoadFloat3x3(&model) *
		DirectX::XMMatrixRotationRollPitchYaw(yRot, zRot, xRot) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
