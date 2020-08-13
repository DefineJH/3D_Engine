#include "Mesh.h"
#include "ImGUI/imgui.h"

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
			root.ShowTree(nodeIndex , selectedIndex);

			ImGui::NextColumn();
			ImGui::Text("Rotation");
			ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

			ImGui::Text("Translation");
			ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
		}
		ImGui::End();
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) *
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		return transform;
	}
private:
	std::optional<int> selectedIndex;
	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	}pos;
};

Model::Model(Graphics& gfx, const std::string fileName)
{
	Assimp::Importer imp;
	const aiScene* pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
	);
	if (!pScene)
	{
		assert("Cannot Import Models" && false);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));
	}
	pRoot = ParseNode(*pScene->mRootNode);
	pModelWindow = std::make_unique<class ModelWindow>();

}

Model::~Model() noexcept
{

}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh)
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

	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, psc, 1u));
	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node)
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
	auto pNode = std::make_unique<Node>(node.mName.C_Str(),std::move(curMeshPtrs), transform);
	//자식 노드 생성을 위해 현재 노드의 자식노드 개수만큼 재귀함수 ㄱㄱ
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
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
	pRoot->Draw(gfx, pModelWindow->GetTransform());
}

Node::Node(const std::string & Name, std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX & transform) noexcept
	:
	name(Name),
	meshPtrs(std::move(MeshPtrs))
{
	DirectX::XMStoreFloat4x4(&(*this).transform, transform);
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
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

void Node::ShowTree(int& nodeIndex, std::optional<int>& selectedIndex) const noexcept
{
	//현재 노드의 인덱스를 저장
	int curNodeIndex = nodeIndex;
	//레퍼런스로 받아왔으니 재귀함수를 위해 다음 값으로 이동
	nodeIndex++;

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow // Arrow클릭시 펼쳐지는 플래그
		| ((curNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0) //현재 노드의 인덱스가 selectedIndex와 같을때, -1은 아무것도 선택 x 선택플래그
		| ((childs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0); //현재 노드의 자식이 없을 때 Leaf노드로 설정하는 플래그
	//void* intptr_t
	if (ImGui::TreeNodeEx(reinterpret_cast<void*>(&curNodeIndex),flag,name.c_str()))
	{
		selectedIndex = ImGui::IsItemClicked() ? curNodeIndex : selectedIndex;
		for (const auto& pChild : childs)
		{
			pChild->ShowTree(nodeIndex,selectedIndex);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childs.push_back(std::move(pChild));
}

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtr)
{
	if (!IsStaticInitalized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx));
	}

	//model로부터 받은 bindPtr들을 전부받아 묶는다
	for (auto& pb : bindPtr)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
			//원래 가지고 있던 소유권을 해제함
			pb.release();
		}
		else
			AddBind(std::move(pb));
	}
	//하나의 메쉬마다 서로다른 포지션값을 가지기 때문에
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
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
