#pragma once
#include <vector>
#include <DirectXMath.h>
#include <type_traits>

namespace DynamicVertex
{
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
			Tangent,
			BiTangent,
			Float3Color,
			Float4Color,
			BGRAColor,
		};
		template<ElementType> struct Map;
		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			//static constexpr로 설정하지 않으면 런타임에 실제 객체를 생성해야하기 때문에 static으로 생성해 메모리를 할당
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};
		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};
		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};
		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N3";
		};
		template<> struct Map<Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "T3";
		};
		template<> struct Map<BiTangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "BiTangent";
			static constexpr const char* code = "BT3";
		};
		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};
		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};
		template<> struct Map<BGRAColor>
		{
			using SysType = DynamicVertex::BGRAColor;
			static constexpr DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
		};
		class Element
		{
		public:
			Element(ElementType type, size_t offset)
				:
				type(type), offset(offset)
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
					return sizeof(Map<Position2D>::SysType);
				case VertexLayout::Position3D:
					return sizeof(Map<Position3D>::SysType);
				case VertexLayout::Texture2D:
					return sizeof(Map<Texture2D>::SysType);
				case VertexLayout::Normal:
					return sizeof(Map<Normal>::SysType);
				case VertexLayout::Tangent:
					return sizeof(Map<Tangent>::SysType);
				case VertexLayout::BiTangent:
					return sizeof(Map<BiTangent>::SysType);
				case VertexLayout::Float3Color:
					return sizeof(Map<Float3Color>::SysType);
				case VertexLayout::Float4Color:
					return sizeof(Map<Float4Color>::SysType);
				case VertexLayout::BGRAColor:
					return sizeof(Map<BGRAColor>::SysType);
				default:
					assert("Invalid elem type" && false);
				}
			}
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
			{
				switch (type)
				{
				case Position2D:
					return GenerateDesc<Position2D>(GetOffset());
				case Position3D:
					return GenerateDesc<Position3D>(GetOffset());
				case Texture2D:
					return GenerateDesc<Texture2D>(GetOffset());
				case Normal:
					return GenerateDesc<Normal>(GetOffset());
				case Tangent:
					return GenerateDesc<Tangent>(GetOffset());
				case BiTangent:
					return GenerateDesc<BiTangent>(GetOffset());
				case Float3Color:
					return GenerateDesc<Float3Color>(GetOffset());
				case Float4Color:
					return GenerateDesc<Float4Color>(GetOffset());
				case BGRAColor:
					return GenerateDesc<BGRAColor>(GetOffset());
				}
				assert("Invalid element type" && false);
				return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}

			const char* GetElemCode() const
			{
				switch (type)
				{
				case Position2D:
					return Map<Position2D>::code;
				case Position3D:
					return Map<Position3D>::code;
				case Texture2D:
					return Map<Texture2D>::code;
				case Normal:
					return Map<Normal>::code;
				case Tangent:
					return Map<Tangent>::code;
				case BiTangent:
					return Map<BiTangent>::code;
				case Float3Color:
					return Map<Float3Color>::code;
				case Float4Color:
					return Map<Float4Color>::code;
				case BGRAColor:
					return Map<BGRAColor>::code;
				}
				return "";
			}
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept(!IS_DEBUG)
			{
				return { Map<type>::semantic , 0 , Map<type>::format , 0 , (UINT)offset ,D3D11_INPUT_PER_VERTEX_DATA,0 };
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
		VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG)
		{
			elements.emplace_back(type, Size());
			return *this;
		}
		//맨뒤의 엘리먼트의 offset을 더한 총 사이즈를 알아오면 현재 레이아웃의 사이즈를 알 수 있음
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return elements.empty() ? 0u : elements.back().GetOffsetAfter();
		}
		size_t GetElementCount() const noexcept
		{
			return elements.size();
		}
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
			desc.reserve(GetElementCount());
			for (const auto& e : elements)
			{
				desc.push_back(e.GetDesc());
			}
			return desc;
		}
		std::string GetLayoutCode() const noexcept
		{
			std::string returnCode;
			for (const Element& e : elements)
			{
				returnCode += e.GetElemCode();
			}
			return returnCode;
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
			pData(pData), layout(layout) {
			assert(pData != nullptr);
		}
		template<VertexLayout::ElementType Type>
		//접근하고 싶은 elemtype 지정하는템플릿 
		auto& Attr() noexcept(!IS_DEBUG)
		{
			//실제 버텍스 내부 정보서 해당 타입의 정보가 저장되 이는 공간으로 offset이동
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
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
				SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Tangent:
				SetAttribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::BiTangent:
				SetAttribute<VertexLayout::BiTangent>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Bad Element type" && false);
			}
		}
		//safety checking
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttr, SrcType&& val) noexcept(!IS_DEBUG)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr(std::is_assignable<Dest, SrcType>::value)
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

	//read-only vertex
	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
			: v(v)
		{}
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept(!IS_DEBUG)
		{
			return const_cast<Vertex&>(v).Attr<Type>();
		}
	private:
		Vertex v;
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
		const char* GetData() const noexcept(!IS_DEBUG)
		{
			return buffer.data();
		}
		//vertex의 개수!
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return buffer.size() / layout.Size();
		}
		size_t SizeBytes() const noexcept(!IS_DEBUG)
		{
			return buffer.size();
		}
		template<class ...Params>
		//param에 2개 이상의 파라미터가 들어가지 않으면 쪼개지지 않는다
		void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
		{
			assert(sizeof...(params) == layout.GetElementCount() && "Param Count Doesnt match with Layout");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		ConstVertex Back() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Back();
		}
		ConstVertex Front() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}
		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->operator[](i);
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
}

