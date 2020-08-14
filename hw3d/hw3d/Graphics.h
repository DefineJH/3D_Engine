#pragma once
#include "ChiliWin.h"
#include "JHException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <random>


class Graphics
{
	//bindable을 friend class로 등록함으로써 Graphics 내의 객체에 접근이 가능케 함
	friend class Bindable;
public:
	class Exception : public JHException
	{
		//constructor override
		using JHException::JHException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorInfo() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	class DeviceRemovedException : public HrException
	{
		//constructor override
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	void EndFrame();
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void BeginFrame(float red, float green, float blue);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;


	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;

	void SetCamera(DirectX::FXMMATRIX CamMat) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

private:
	bool imguiEnabled = true;
	DirectX::XMMATRIX projectionMat;
	DirectX::XMMATRIX cameraMat;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSView;
	IDXGIAdapter* pAdapter = nullptr;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG

};

