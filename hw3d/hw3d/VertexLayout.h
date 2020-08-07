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

//vertex에 들어가는 element들을 알려주는 클래스
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
	//elements에 들어있는 타입을 전부돌며 이 함수를 부를때 템플릿 매개변수로 지정한 타입과 맞는지 검사후
	//매개변수로 지정한 타입과 같은 e를 반환한다 
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
	//맨뒤의 엘리먼트의 offset을 더한 총 사이즈를 알아오면 현재 레이아웃의 사이즈를 알 수 있음
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return elements.empty() ? 0u : elements.back.GetOffsetAfter();
	}
private:
	std::vector<Element> elements;
};

//vertex layout를 참조하며 vertex버퍼의 포인터를 가지고 실제 데이터를 저장할 수 있게하는 일종의 팩토리 클래스
class Vertex
{
	friend class VertexLayout;
public:
	Vertex(char* pData, VertexLayout& layout)
		:
		pData(pData), layout(layout) {}
	template<VertexLayout::ElementType Type>
	//접근하고 싶은 elemtype지정하는템플릿 
	auto& Attr() noexcept(!IS_DEBUG)
	{
		//layout이 resolve해서 얻은 element객체
		const auto& element = layout.Resolve<Type>();
		//실제 버텍스 내부 정보서 해당 타입의 정보가 저장되 이는 공간으로 offset이동
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
	//템플릿 재귀함수
	//Emplace에서 들어온 가변매개변수들을 1 , n-1로 계속 쪼개어 1은 저장 , n-1 은 -> 1 : n-2 로 계속 변환
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

//실제 데이터를 담을 수 있는 클래스
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
	//vertex의 개수!
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return buffer.size() / layout.Size();
	}
	template<class ...Params>
	//param에 2개 이상의 파라미터가 들어가지 않으면 쪼개지지 않는다
	void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
	{
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...)
		
	}
	Vertex Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		//버퍼의 끝 - 1버텍스  = 마지막 버텍스
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
	//vertex를 byte로 저장한다 결국에 푸는법은 vertexlayout에 존재하기 때문에
	std::vector<char> buffer;
	VertexLayout layout;
};