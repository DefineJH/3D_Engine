#pragma once
#include "Bindable.h"

class Topology :
	public Bindable
{
public:
	Topology(Graphics& gfx, D3D_PRIMITIVE_TOPOLOGY tp = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Bind(Graphics& gfx) noexcept override;
private:
	D3D_PRIMITIVE_TOPOLOGY tp;
};

