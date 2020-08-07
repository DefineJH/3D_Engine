#pragma once
#include <vector>
#include <DirectXMath.h>
#include <type_traits>

struct BGRAColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

//vertex�� ���� element���� �˷��ִ� Ŭ����
class VertexLayout
{
public:
	enum ElementType
	{
		Position2D,
		Position3D,
		Texture2D,
		Normal,
		Float3Color,
		Float4Color,
		BGRAColor,
	};
	class Element
	{
	public:
		Element(ElementType type, size_t offset)
			:
			type(type) , offset(offset)
		{}
		size_t GetOffsetAfter() const noexcept(!IS_DEBUG)
		{
			return offset + Size();
		}
		size_t GetOffset() const
		{
			return offset;
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return SizeOf(type);
		}
		ElementType GetType() const noexcept(!IS_DEBUG)
		{
			return type;
		}
		static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
		{
			switch (type)
			{
			case VertexLayout::Position2D:
				return sizeof(DirectX::XMFLOAT2);
			case VertexLayout::Position3D:
				return sizeof(DirectX::XMFLOAT3);
			case VertexLayout::Texture2D:
				return sizeof(DirectX::XMFLOAT2);
			case VertexLayout::Normal:
				return sizeof(DirectX::XMFLOAT3);
			case VertexLayout::Float3Color:
				return sizeof(DirectX::XMFLOAT3);
			case VertexLayout::Float4Color:
				return sizeof(DirectX::XMFLOAT4);
			case VertexLayout::BGRAColor:
				return sizeof(DirectX::XMFLOAT4);
			default:	
				assert("Invalid elem type" && false);
			}
		}
	private:
		ElementType type;
		size_t offset;
	};

public:
	//elements�� ����ִ� Ÿ���� ���ε��� �� �Լ��� �θ��� ���ø� �Ű������� ������ Ÿ�԰� �´��� �˻���
	//�Ű������� ������ Ÿ�԰� ���� e�� ��ȯ�Ѵ� 
	template<ElementType Type>
	const Element& Resolve() const noexcept(!IS_DEBUG)
	{
		for (auto& e : elements)
		{
			if (e.GetType() == Type)
			{
				return e;
			}
		}
		assert("Could not resolve elem type" && false);
		return elements.front();
	}
	const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
	{
		return elements[i];
	}
	template<ElementType Type>
	VertexLayout& Append() noexcept(!IS_DEBUG)
	{
		elements.emplace_back(Type, Size());
		return *this;
	}
	//�ǵ��� ������Ʈ�� offset�� ���� �� ����� �˾ƿ��� ���� ���̾ƿ��� ����� �� �� ����
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return elements.empty() ? 0u : elements.back.GetOffsetAfter();
	}
private:
	std::vector<Element> elements;
};

//vertex layout�� �����ϸ� vertex������ �����͸� ������ ���� �����͸� ������ �� �ְ��ϴ� ������ ���丮 Ŭ����
class Vertex
{
	friend class VertexLayout;
public:
	Vertex(char* pData, VertexLayout& layout)
		:
		pData(pData), layout(layout) {}
	template<VertexLayout::ElementType Type>
	//�����ϰ� ���� elemtype�����ϴ����ø� 
	auto& Attr() noexcept(!IS_DEBUG)
	{
		//layout�� resolve�ؼ� ���� element��ü
		const auto& element = layout.Resolve<Type>();
		//���� ���ؽ� ���� ������ �ش� Ÿ���� ������ ����� �̴� �������� offset�̵�
		auto pAttribute = pData + element.GetOffset();
		if (Type == VertexLayout::ElementType::Position2D)
		{
			return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::Position3D)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::Texture2D)
		{
			return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::Normal)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::Float3Color)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::Float4Color)
		{
			return *reinterpret_cast<XMFLOAT4*>(pAttribute);
		}
		else if (Type == VertexLayout::ElementType::BGRAColor)
		{
			return *reinterpret_cast<XMFLOAT4*>(pAttribute);
		}
	}
	//���ø� ����Լ�
	//Emplace���� ���� �����Ű��������� 1 , n-1�� ��� �ɰ��� 1�� ���� , n-1 �� -> 1 : n-2 �� ��� ��ȯ
	template<typename First, typename ...Rest>
	void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG)
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
	}
	template<class T>
	void SetAttributeByIndex(size_t i, T&& val) noexcept(!IS_DEBUG)
	{
		const auto& elem = layout.ResolveByIndex(i);
		auto pAttribute = pData + elem.GetOffset();
		switch (elem.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Position3D:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<XMFLOAT4>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<XMFLOAT4>(pAttribute, std::forward<T>(val));
			break;
		}
	}
	//safety checking
	template<typename Dest , typename Src>
	void SetAttribute(char* pAttr, Src&& val) noexcept(!IS_DEBUG)
	{
		if (std::is_assignable<Dest, Src>::value)
		{
			*reinterpret_cast<Dest*>(pAttr) = val;
		}
		else
			assert("Param attr type mismatch" && false);
	}
private:
	char* pData = nullptr;
	const VertexLayout& layout;
};

//���� �����͸� ���� �� �ִ� Ŭ����
class VertexBuffer
{
public:
	VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
		: layout(std::move(layout))
	{}
	const VertexLayout& GetLayout() const noexcept(!IS_DEBUG)
	{
		return layout;
	}
	//vertex�� ����!
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return buffer.size() / layout.Size();
	}
	template<class ...Params>
	//param�� 2�� �̻��� �Ķ���Ͱ� ���� ������ �ɰ����� �ʴ´�
	void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
	{
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...)
		
	}
	Vertex Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		//������ �� - 1���ؽ�  = ������ ���ؽ�
		return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
	}
	Vertex Front() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() , layout };
	}
	Vertex operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i , layout };
	}
private:
	//vertex�� byte�� �����Ѵ� �ᱹ�� Ǫ�¹��� vertexlayout�� �����ϱ� ������
	std::vector<char> buffer;
	VertexLayout layout;
};