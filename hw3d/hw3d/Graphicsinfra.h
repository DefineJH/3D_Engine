#pragma once
#include <dxgi.h>
#include <wrl.h>
#include <vector>
#include <string.h>
#include <string>

class GraphicsInfra
{
public:
	GraphicsInfra();

	std::vector<std::wstring> GetGraphicsDesc();
	IDXGIAdapter* SelectByIndex(int idx);
private:
	Microsoft::WRL::ComPtr<IDXGIFactory> pDxgiFactory;
	std::vector<IDXGIAdapter*> adapters;
};

