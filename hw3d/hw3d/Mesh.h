#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtr);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node(const std::string& Name, std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX& transform) noexcept;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	//루트노드의 이 함수를 호출시, child노드까지 재귀함수를 돌려 하이어라키를 Imgui로 그린다
	//**nodeIndex - node마다 가지고 있는 유니크한 인덱스
	//**selectedIndex - 현재 선택되어있는 인덱스를 가지고 있는 optional객체
	void ShowTree(int& nodeIndex, std::optional<int>& selectedIndex) const noexcept;
private:
	//model에서만 넣을 수 있다.
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
private:
	//하나의 노드는 복수의 자식노드와 메쉬들을 가질 수 있고 고유의 트랜스폼 존재
	std::vector<std::unique_ptr<Node>> childs;
	std::vector<Mesh*> meshPtrs;
	std::string name;
	DirectX::XMFLOAT4X4 transform;
};
class Model
{
public:
	Model(Graphics& gfx, const std::string fileName);
	~Model()noexcept;
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node);
	void ShowWindow(const char* WindowName) noexcept;
	void Draw(Graphics& gfx)const;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pModelWindow;
};
