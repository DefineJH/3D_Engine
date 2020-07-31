#include "DxgiInfoManager.h"
#include "Graphics.h"
#include "Window.h"
#include "GraphicsThrowMacro.h"
#include "WindowsThrowMacro.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void **); // 함수 포인터를 정의
	//dll 로드
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw JHWND_LAST_EXCEPT();
	}

	//dll에서 사용하고자 하는 함수 로드 
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	//함수를 dll서 가져오지 못했을시에
	if (DxgiGetDebugInterface == nullptr)
	{
		throw JHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&pDxgiInfoQueue)));
}

DxgiInfoManager::~DxgiInfoManager()
{
	if (pDxgiInfoQueue != nullptr)
	{
		pDxgiInfoQueue->Release();
	}
}

void DxgiInfoManager::Set() noexcept
{
	//next를 메시지의 개수로 설정함으로써 set을 호출한 시점부터의 메시지를 GetMessage서 부를 수 있다
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		//메시지의 크기를 messageLength에 대입
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessageA(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		//알아온 message 크기만큼 버퍼를 할당
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// 메시지를 버퍼에 할당
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}
