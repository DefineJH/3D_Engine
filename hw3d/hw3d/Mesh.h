#pragma once
#include "DrawableBase.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class ModelException : public JHException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept; 
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;	
};

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
	friend class ModelWindow;
public:
	Node(const std::string& Name, std::vector<Mesh*> MeshPtrs, const DirectX::XMMATRIX& transform) noexcept;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
private:
	//��Ʈ����� �� �Լ��� ȣ���, child������ ����Լ��� ���� ���̾��Ű�� Imgui�� �׸���
	//**nodeIndex - node���� ������ �ִ� ����ũ�� �ε���
	//**selectedIndex - ���� ���õǾ��ִ� �ε����� ������ �ִ� optional��ü
	//**pSelectedNode  - ���� ���õǾ� �ִ� ��带 ������ �� �� ����
	void ShowTree(int& nodeIndex, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;
	//model������ ���� �� �ִ�.
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
private:
	//�ϳ��� ���� ������ �ڽĳ��� �޽����� ���� �� �ְ� ������ Ʈ������ ����
	std::vector<std::unique_ptr<Node>> childs;
	std::vector<Mesh*> meshPtrs;
	std::string name; 
	DirectX::XMFLOAT4X4 transform;
	//������ Ʈ������
	DirectX::XMFLOAT4X4 appliedTransform;
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