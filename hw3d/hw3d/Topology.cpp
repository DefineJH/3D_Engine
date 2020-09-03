#include "Topology.h"
#include "GraphicsThrowMacro.h"
#include "BindableCodex.h"

Topology::Topology(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp)
	: tp(tp)
{
}

void Topology::Bind(Graphics& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(tp);
}

std::shared_ptr<Topology> Topology::Resolve(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp)
{
	return Codex::Resolve<Topology>(gfx, tp);
}

std::string Topology::GenerateUID(D3D_PRIMITIVE_TOPOLOGY tp)
{
	return "Topology" + std::to_string(tp);
}

std::string Topology::GetUID() const noexcept
{
	return GenerateUID(tp);
}
