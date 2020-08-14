#include "Mesh.h"
#include "ImGUI/imgui.h"
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
			root.ShowTree(nodeIndex , selectedIndex , pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& pos = transforms[*selectedIndex];
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
		const auto& transform = transforms.at(*selectedIndex);
		return DirectX::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
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
	);
	if (!pScene)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
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
	auto pNode = std::make_unique<Node>(node.mName.C_Str(),std::move(curMeshPtrs), transform);
	//�ڽ� ��� ������ ���� ���� ����� �ڽĳ�� ������ŭ ����Լ� ����
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
	if (auto node = pModelWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pModelWindow->GetTransform());
	}
	pRoot->Draw(gfx, DirectX::XMMatrixIdentity());
}

Node::Node(const std::string & Name, std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX & transform) noexcept
	:
	name(Name),
	meshPtrs(std::move(MeshPtrs))
{
	DirectX::XMStoreFloat4x4(&(*this).transform, transform);
	DirectX::XMStoreFloat4x4(&(*this).appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	//applied tr = ui�� �ٲ��� transform
	//transform = node�� ������ �ִ� ������ǥ
	//accumulatedTr = ������弭 ���Ǹ鼭 ������ ������ǥ
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

void Node::ShowTree(int& nodeIndex, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
{
	//���� ����� �ε����� ����
	int curNodeIndex = nodeIndex;
	//���۷����� �޾ƿ����� ����Լ��� ���� ���� ������ �̵�
	nodeIndex++;

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow // ArrowŬ���� �������� �÷���
		| ((curNodeIndex == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0) //���� ����� �ε����� selectedIndex�� ������, -1�� �ƹ��͵� ���� x �����÷���
		| ((childs.empty()) ? ImGuiTreeNodeFlags_Leaf : 0); //���� ����� �ڽ��� ���� �� Leaf���� �����ϴ� �÷���
	//void* intptr_t
	const auto expanded = ImGui::TreeNodeEx(reinterpret_cast<void*>(&curNodeIndex), flag, name.c_str());
	if (ImGui::IsItemClicked())
	{
		selectedIndex = curNodeIndex;
		pSelectedNode = const_cast<Node*>(this);
	}
	if (expanded)
	{
		for (const auto& pChild : childs)
		{
			pChild->ShowTree(nodeIndex, selectedIndex, pSelectedNode);
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