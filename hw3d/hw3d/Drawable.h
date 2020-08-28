#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
class Bindable;
class IndexBuffer;

class Drawable
{

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
public:
	//transform constant buffer���� �Ѱ��ֱ� ���� ������Ʈ�� ��������� ��ǥ
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
protected:
	template<typename T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
	void AddBind(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG);
private:
	// �ε��� ���۸� ������ ���� , �ֳ��ϸ� �ε������۾��� �ε��� ������ �˾ƾ� drawindexed ����
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> binds;
};

