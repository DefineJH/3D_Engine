#include "Graphicsinfra.h"
#include <memory.h>
GraphicsInfra::GraphicsInfra()
{
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)pDxgiFactory.GetAddressOf());
}


std::vector<std::wstring> GraphicsInfra::GetGraphicsDesc()
{
	std::vector<std::wstring> desc;
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC aDesc;
	for (UINT i = 0;
		pDxgiFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(pAdapter);
		pAdapter->GetDesc(&aDesc);
		desc.push_back(aDesc.Description);
	}
	return std::move(desc);
}

IDXGIAdapter* GraphicsInfra::SelectByIndex(int idx)
{
	return adapters[idx];
}

