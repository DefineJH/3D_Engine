#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtr)
	{
		if (!IsStaticInitalized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx));
		}

		//model�κ��� ���� bindPtr���� ���ι޾� ���´�
		for (auto& pb : bindPtr)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
				//���� ������ �ִ� �������� ������
				pb.release();
			}
			else
				AddBind(std::move(pb));
		}
		//�ϳ��� �޽����� ���δٸ� �����ǰ��� ������ ������
		AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	}
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
		Drawable::Draw(gfx);
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&transform);
	}
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node(std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX& transform) noexcept
		:
		meshPtrs(std::move(MeshPtrs))
	{
		DirectX::XMStoreFloat4x4(&(*this).transform, transform);
	}
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		const auto CurNodeTr = DirectX::XMLoadFloat4x4(&transform) * accumulatedTransform;
		for (Mesh* mesh : meshPtrs)
		{
			mesh->Draw(gfx, CurNodeTr);
		}
		for (const std::unique_ptr<Node>& childnode : childs)
		{
			childnode->Draw(gfx, CurNodeTr);
		}
	}
private:
	//model������ ���� �� �ִ�.
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		childs.push_back(std::move(pChild));
	}
private:
	//�ϳ��� ���� ������ �ڽĳ��� �޽����� ���� �� �ְ� ������ Ʈ������ ����
	std::vector<std::unique_ptr<Node>> childs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};
class Model
{
public:
	Model(Graphics& gfx, const std::string fileName)
	{
		Assimp::Importer imp;
		const aiScene* pScene = imp.ReadFile(fileName.c_str(),
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
		);

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
		}
		pRoot = ParseNode(*pScene->mRootNode);
	}
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh)
	{
		DynamicVertex::VertexBuffer vBuf(DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D).Append(DynamicVertex::VertexLayout::Normal));
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vBuf.EmplaceBack(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const aiFace& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vBuf));
		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		auto pVertexShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetBytecode();

		bindablePtrs.push_back(std::move(pVertexShader));
		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vBuf.GetLayout().GetD3DLayout(), pVertexShaderByteCode));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color = { 0.6f , 0.6f , 0.8f };
			float specularInten = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		};

		PSMaterialConstant psc;

		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, psc ,1u));
		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}
	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		//row major -> column major
		const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
			reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
		));
		
		//���� ��尡 ������ �޽��� ������ ��ü
		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(node.mNumMeshes);
		for (size_t i = 0; i < node.mNumMeshes; i++)
		{
			//����� mesh ���ʹ� scene��ü���� �����ϴ� mMeshes�� �ε����� �����س��´�
			const auto meshIdx = node.mMeshes[i];
			//���� �޽��� �ε����� ��ü�޽��� ������ meshptr�� �޾ƿ� push_back�Ѵ�
			curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
		}
		//���� ��� ����
		auto pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
		//�ڽ� ��� ������ ���� ���� ����� �ڽĳ�� ������ŭ ����Լ� ����
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(*node.mChildren[i]));
		}
		return pNode;
	}
	void Draw(Graphics& gfx, DirectX::FXMMATRIX mt)const
	{
		pRoot->Draw(gfx,mt);
	}
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};