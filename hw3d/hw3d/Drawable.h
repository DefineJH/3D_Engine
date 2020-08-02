#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
class Bindable;

class Drawable
{
	//GetStaticBinds에 접근해야 하므로 어쩔수 없는 커플링 -> 하지만 Drawable과 DrawableBase는 밀접한 관계에 놓이므로 어느정도의 커플링은 허용가능
	template<typename T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
public:
	//transform constant buffer에게 넘겨주기 위한 오브젝트의 월드공간의 좌표
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
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
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuf) noexcept;
protected:
	//가상함수로 만드는 이유, 실제 entity는 drawable base로 생성되므로 GetStaticBind가 구현되있을 것이므로
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	// 인덱스 버퍼를 가지는 이유 , 왜냐하면 인덱스버퍼안의 인덱스 개수를 알아야 drawindexed 가능
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};

