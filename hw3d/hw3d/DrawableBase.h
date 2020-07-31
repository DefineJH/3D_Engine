#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

//���� entity ( box , sphere, cylinder ) ����� ���� �ε����� ���ؽ� ��ǥ�� ������ �ִ�.
//�̸� �����ϱ� ���� entity���� �ٸ� static ������ �Լ��� ����� ����� �� ������. ����1 �巴�� ������.
//�׸��� ����2 drawable�� draw �Լ��� �� ��ü���� ����ǹǷ� static�� ���� ������ ����.
//�̷� ��Ȳ���� box�� static indexbuffer�� �־��ְ� �̸� �� ��ü���� bind�� ���� �����͸� ������ ���´�. ��¯ ���̴�.

// ����1 �ذ� - �߰�Ŭ������ ���ø�ȭ�Ͽ� ����� ���ø�ȭ�Ͽ� ��ӹ��� ��, �Ź� �ڵ带 �ٿ��ֱ� �����ʰ�
// entity���� ���ο� static���� ������. templateƯ��ȭ! ����ؿ�

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
	//���� ���� ��ü���� AddStaticIndexBuffer�� ȣ���� �� 1���� �ϰ� �ִ� �׷����ϸ� �츮�� �θ�Ŭ������ pIndexbuffer�� �ʱ�ȭ�� �� �����Ƿ�
	//���� ��ü���� bind���� �ʱ�ȭ �� �� �ٸ� ��ü ������ �� �Լ��� �θ���
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempt to add index buffer twice" && pIndexBuffer == nullptr);
		for (const auto& b : staticBinds)
		{
			// ���� ĳ������ ������ ���
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
