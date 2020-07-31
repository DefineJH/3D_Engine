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
	//DrawableBase<T>의 GetStaticBinds를 불러온다
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("무조건 인덱스 버퍼를 바인드 할 때는 AddIndexBuffer 사용해야 함" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back( std::move(bind) );
}

void Drawable::AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuf) noexcept
{
	assert("인덱스 버퍼를 두번 묶지 마시오" && pIndexBuffer == nullptr);
	pIndexBuffer = indexBuf.get();
	binds.push_back(std::move(indexBuf));
}
