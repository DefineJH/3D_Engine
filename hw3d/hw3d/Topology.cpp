#include "Topology.h"
#include "GraphicsThrowMacro.h"

Topology::Topology(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp)
	: tp(tp)
{
}

void Topology::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(tp);
}
