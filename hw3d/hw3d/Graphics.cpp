#include "Graphics.h"
#include "dxerr.h"
#include "Window.h"
#include "GraphicsThrowMacro.h"
#include "ImGUI/imgui_impl_dx11.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


//d3d11 라이브러리 임포트
#pragma comment(lib, "d3d11.lib")
//cso파일 로딩위해서
#pragma comment(lib, "D3DCompiler.lib")

//dxgi 라이브러리 로딩
#pragma comment(lib, "dxgi.lib")

int DisplayConfirmSaveAsMessageBox(HWND &hWnd, wchar_t* CardDesc)
{
	int msgboxID = MessageBoxW(
		hWnd,
		CardDesc,
		L"Confirm Save As",
		MB_ICONEXCLAMATION | MB_YESNO
	);

	if (msgboxID == IDYES)
	{
		return 1;
	}

	return 0;
}
Graphics::Graphics(HWND hWnd, int width , int height)
{
	HRESULT hr;

	IDXGIFactory * pFactory = NULL;
	GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));

	std::vector<IDXGIAdapter*> adapters;

	IDXGIAdapter* pAdapter = nullptr;
	for (UINT i = 0;
		pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(pAdapter);
	}

	for (int j = 0; j < adapters.size(); j++)
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		adapters[j]->GetDesc(&AdapterDesc);
		if (DisplayConfirmSaveAsMessageBox(hWnd, AdapterDesc.Description))
		{
			pAdapter = adapters[j];
		}
	}
	DXGI_SWAP_CHAIN_DESC sd = {};
	//hWnd를 보고 알아서 알아보라는 뜻
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//anti-aliasing 관련
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	//버퍼의 사용용도
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//double buffering 
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//HRESULT hr;

	//device & swapchain생성
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	));


	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	//comptr의 &는 Release를 호출하고 주소를 반환한다.  ( ReleaseAndGetAddressOf() )
	//고로 comptr이 잡고있는 객체의 주소를 가리키는 주소를 얻고 싶다면 GetAddressOf() 사용하자
	GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = (UINT)width;
	depthDesc.Height = (UINT)height;
	depthDesc.MipLevels = 1u;
	depthDesc.ArraySize = 1u;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1u;
	depthDesc.SampleDesc.Quality = 0u;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;

	GFX_THROW_INFO(pDevice->CreateTexture2D(&depthDesc, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsv, &pDSView));

	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSView.Get());

	//뷰포트 설정
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());

}


void Graphics::EndFrame()
{
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		//드라이버 크래쉬, 오버클럭등의 문제로 인해 발생하는 에러
		if(hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}


void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::BeginFrame(float red, float green, float blue)
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projectionMat = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projectionMat;
}

std::vector<std::wstring> Graphics::GetGraphicCard()
{
	HRESULT hr;

	IDXGIFactory * pFactory = NULL;
	GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory));

	std::vector<IDXGIAdapter*> adapters;
	std::vector<std::wstring> descVec;
	IDXGIAdapter* pAdapter = nullptr;
	for (UINT i = 0;
		pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(pAdapter);
	}

	for (int j = 0; j < adapters.size(); j++)
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		adapters[j]->GetDesc(&AdapterDesc);
		descVec.push_back(AdapterDesc.Description);
	}
	return descVec;
}

void Graphics::EnableImGui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImGui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImGuiEnabled() const noexcept
{
	return imguiEnabled;
}

void Graphics::SetCamera(DirectX::FXMMATRIX CamMat) noexcept
{
	cameraMat = CamMat;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return cameraMat;
}

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char * file, HRESULT hr , std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
		if (!info.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		}
		oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "JH Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "JH Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

