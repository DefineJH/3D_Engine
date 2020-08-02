#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
class Bindable;

class Drawable
{
	//GetStaticBinds�� �����ؾ� �ϹǷ� ��¿�� ���� Ŀ�ø� -> ������ Drawable�� DrawableBase�� ������ ���迡 ���̹Ƿ� ��������� Ŀ�ø��� ��밡��
	template<typename T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
public:
	//transform constant buffer���� �Ѱ��ֱ� ���� ������Ʈ�� ��������� ��ǥ
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
	//�����Լ��� ����� ����, ���� entity�� drawable base�� �����ǹǷ� GetStaticBind�� ���������� ���̹Ƿ�
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	// �ε��� ���۸� ������ ���� , �ֳ��ϸ� �ε������۾��� �ε��� ������ �˾ƾ� drawindexed ����
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};

