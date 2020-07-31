#include "Drawable.h"
#include "GraphicsThrowMacro.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics & gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	//DrawableBase<T>�� GetStaticBinds�� �ҷ��´�
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("������ �ε��� ���۸� ���ε� �� ���� AddIndexBuffer ����ؾ� ��" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back( std::move(bind) );
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuf) noexcept
{
	assert("�ε��� ���۸� �ι� ���� ���ÿ�" && pIndexBuffer == nullptr);
	pIndexBuffer = indexBuf.get();
	binds.push_back(std::move(indexBuf));
}
