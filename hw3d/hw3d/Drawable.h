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
	//transform constant buffer에게 넘겨주기 위한 오브젝트의 월드공간의 좌표
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
	// 인덱스 버퍼를 가지는 이유 , 왜냐하면 인덱스버퍼안의 인덱스 개수를 알아야 drawindexed 가능
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> binds;
};

