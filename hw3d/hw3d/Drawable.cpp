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

	gfx.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Index Buffer already binded" && pIndexBuffer == nullptr);
		pIndexBuffer =  &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back( std::move(bind) );
}
