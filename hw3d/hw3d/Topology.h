#pragma once
#include "Bindable.h"

class Topology :
	public Bindable
{
public:
	Topology(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Bind(Graphics& gfx) noexcept override;
	static std::shared_ptr<Topology> Resolve(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp);
	static std::string GenerateUID(D3D_PRIMITIVE_TOPOLOGY tp);
	std::string GetUID() const noexcept override;
private:
	D3D_PRIMITIVE_TOPOLOGY tp;
};

