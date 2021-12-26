#include "Mesh.h"
#include "ImGUI/imgui.h"
#include "Surface.h"
#include "Bindablebase.h"
#include <stdexcept>
#include <unordered_map>

class ModelWindow
{
public:
	void Show(const char* WindowName , const Node& root)
	{
		WindowName = WindowName ? WindowName : "Model";
		int nodeIndex = 0;
		if (ImGui::Begin(WindowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& pos = transforms[pSelectedNode->GetID()];
				ImGui::Text("Rotation");
				ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

				ImGui::Text("Translation");
				ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);

			}
		}
		ImGui::End();
	}
	Node* GetSelectedNode() const noexcept {
		return pSelectedNode;
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetID());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
private:
	Node* pSelectedNode = nullptr;
	struct STransform
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, STransform> transforms;
};

Model::Model(Graphics& gfx, const std::string fileName)
{
	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
	);
	if (!pScene)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}
	int nextId = 0;
	pRoot = ParseNode(nextId ,*pScene->mRootNode);
	pModelWindow = std::make_unique<class ModelWindow>();

}

Model::~Model() noexcept
{

}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using namespace std::string_literals;
	using DynamicVertex::VertexLayout;

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	const auto base = "Models\\Texture\\"s;

	bool hasSpecularMap = false;
	bool hasAlphaGloss = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	float shininess = 2.0f;
	DirectX::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
	DirectX::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };

	//메쉬 내부 머터리얼 처리
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		aiString texFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(Texture::Resolve(gfx, base + texFileName.C_Str()));
			hasDiffuseMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, base + texFileName.C_Str(), 1);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
		}
		if (!hasAlphaGloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, base + texFileName.C_Str(), 2);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasNormalMap = true;
		}

		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
		{
			bindablePtrs.push_back(Sampler::Resolve(gfx));
		}
	}

	const auto meshTag = base + "%" + mesh.mName.C_Str();

	const float scale = 6.0f;

	if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
	{
		DynamicVertex::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::BiTangent)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso"));

		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantFullmonte pmc;
		pmc.specularPower = shininess;
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap && hasNormalMap)
	{
		DynamicVertex::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::BiTangent)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));

		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffnorm
		{
			float specularIntensity;
			float specularPower;
			BOOL  normalMapEnabled = TRUE;
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));
	}
	else if (hasDiffuseMap)
	{
		DynamicVertex::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));

		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		DynamicVertex::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		auto pvsbc = pvs->GetBytecode();
		bindablePtrs.push_back(std::move(pvs));

		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));

		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantNotex pmc;
		pmc.specularPower = shininess;
		pmc.specularColor = specularColor;
		pmc.materialColor = diffuseColor;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material smh");
	}

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId,const aiNode& node)
{
	//row major -> column major
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	//현재 노드가 보유한 메쉬를 저장할 객체
	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		//노드의 mesh 벡터는 scene객체에서 접근하는 mMeshes의 인덱스를 저장해놓는다
		const auto meshIdx = node.mMeshes[i];
		//실제 메쉬의 인덱스를 전체메쉬를 저장한 meshptr서 받아와 push_back한다
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}
	//현재 노드 생성
	auto pNode = std::make_unique<Node>(nextId++,node.mName.C_Str(),std::move(curMeshPtrs), transform);
	//자식 노드 생성을 위해 현재 노드의 자식노드 개수만큼 재귀함수 ㄱㄱ
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId,*node.mChildren[i]));
	}
	return pNode;
}

void Model::ShowWindow(const char* WindowName) noexcept
{
	WindowName = WindowName ? WindowName : "Model";
	pModelWindow->Show(WindowName, *pRoot.get());
}

void Model::Draw(Graphics& gfx) const
{
	if (auto node = pModelWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pModelWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

Node::Node(int id,const std::string & Name, std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX & transform) noexcept
	:
	id(id),
	name(Name),
	meshPtrs(std::move(MeshPtrs))
{
	DirectX::XMStoreFloat4x4(&(*this).transform, transform);
	DirectX::XMStoreFloat4x4(&(*this).appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	//applied tr = ui서 바꿔준 transform
	//transform = node가 가지고 있는 로컬좌표
	//accumulatedTr = 상위노드서 계산되면서 내려온 월드좌표
	const auto CurNodeWorldTr = DirectX::XMLoadFloat4x4(&appliedTransform) * DirectX::XMLoadFloat4x4(&transform) *accumulatedTransform;
	for (Mesh* mesh : meshPtrs)
	{
		mesh->Draw(gfx, CurNodeWorldTr);
	}
	for (const std::unique_ptr<Node>& childnode : childs)
	{
		childnode->Draw(gfx, CurNodeWorldTr);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetID() const noexcept
{
	return id;
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetID();
	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow // Arrow클릭시 펼쳐지는 플래그
		| ((GetID() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0) //현재 노드의 인덱스가 selectedIndex와 같을때, -1은 아무것도 선택 x 선택플래그
		| ((childs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0); //현재 노드의 자식이 없을 때 Leaf노드로 설정하는 플래그
	//void* intptr_t
	const auto expanded = ImGui::TreeNodeEx(reinterpret_cast<void*>(GetID()), flag, name.c_str());
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	if (expanded)
	{
		for (const auto& pChild : childs)
		{
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childs.push_back(std::move(pChild));
}

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtr)
{
	AddBind(std::make_shared<Topology>(gfx));
	//model로부터 받은 bindPtr들을 전부받아 묶는다
	for (auto& pb : bindPtr)
	{
		AddBind(std::move(pb));
	}
	//하나의 메쉬마다 서로다른 포지션값을 가지기 때문에
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	: JHException(line, file) , note(note)
{
}

const char* ModelException::what() const noexcept
{
	return note.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Assimp Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}
