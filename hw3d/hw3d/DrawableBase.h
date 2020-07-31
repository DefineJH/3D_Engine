#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

//같은 entity ( box , sphere, cylinder ) 등등은 같은 인덱스와 버텍스 좌표를 가지고 있다.
//이를 공유하기 위해 entity마다 다른 static 변수와 함수를 만들어 등록할 수 있지만. 문제1 드럽게 귀찮다.
//그리고 문제2 drawable의 draw 함수는 매 객체마다 실행되므로 static에 대한 인지가 없다.
//이런 상황에서 box에 static indexbuffer을 넣어주고 이를 각 객체마다 bind시 같은 데이터를 여러번 묶는다. 말짱 꽝이다.

// 문제1 해결 - 중간클래스를 템플릿화하여 만들어 템플릿화하여 상속받을 시, 매번 코드를 붙여넣기 하지않고
// entity마다 새로운 static들을 만들어낸다. template특수화! 사랑해요

template <typename T>
class DrawableBase :
	public Drawable
{
protected:
	static bool IsStaticInitalized() noexcept
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert(pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	//현재 같은 객체에서 AddStaticIndexBuffer을 호출할 때 1번만 하고 있다 그렇게하면 우리는 부모클래스의 pIndexbuffer를 초기화할 수 없으므로
	//만약 객체에서 bind들이 초기화 된 후 다른 객체 생성시 이 함수를 부른다
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempt to add index buffer twice" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds)
		{
			// 만약 캐스팅이 성공할 경우
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("There is no indexbuffer that in static binds"  && pIndexBuffer != nullptr);

	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<typename T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;
