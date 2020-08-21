#define DIRECTINPUT_VERSION 0x0800
#define ID_COMBOBOX 100
#include "Window.h"
#include "resource.h"
#include "WindowsThrowMacro.h"
#include <sstream>
#include <dinput.h>
#include <WinUser.h>
HWND hComboHandle;
int SelectIdx = 1;
//�̱��� ����ƽ Ŭ���� ����
Window::WindowClass Window::WindowClass::wndClass;

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::WindowClass::WindowClass() noexcept
	:
	//���� �ν��Ͻ��� ���ͼ� �ʱ�ȭ
	hInst( GetModuleHandle(nullptr))
{
	//window class ���
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE( IDI_ICON1), IMAGE_ICON,32,32,0));
	wc.hCursor = NULL;
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	RegisterClassEx(&wc);

}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

Window::Window(int width, int height, const char * name)
	: width(width),height(height)
{

	Adapterdesc = Infra.GetGraphicsDesc();
	RECT wndRect;
	wndRect.left = 100;
	wndRect.right = width + wndRect.left;
	wndRect.top = 100;
	wndRect.bottom = wndRect.top + height;
	// 2��° �Ű������� ������ �ɼǵ��� ���� ���� Ŭ���̾�Ʈ ������ wndRect�� ��ü �������� ũ�⸦ wndRect�� �������ش�
	if (AdjustWindowRect(&wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false) == 0)
	{
		throw JHWND_LAST_EXCEPT();
	}

	hWnd = CreateWindow(WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this);
	//lpParam �� this�� ���������� ���� ������ ��ü�� �ּҸ� ������
	if (hWnd == nullptr)
	{
		throw JHWND_LAST_EXCEPT();
	}


	ShowWindow(hWnd, SW_SHOWDEFAULT);

	ImGui_ImplWin32_Init(hWnd);

	
	//��ü�� Graphic�� ������ �ʱ�ȭ�� �� ���� ������(hWnd�� ���� ������) Unique_ptr�� ������ �־ �ʱ�ȭ������ �����
	pGfx = std::make_unique<Graphics>(hWnd, width, height, Infra.SelectByIndex(SelectIdx));


	//raw mouse ���
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&rid, 1,sizeof(rid)) == FALSE) {
		throw JHWND_LAST_EXCEPT();
	}
	
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
		throw JHWND_LAST_EXCEPT();
}

void Window::EnableCursor() noexcept
{
	m_CursorEnabled = true;
	ShowCursor();
	EnableUIMouseInteraction();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	m_CursorEnabled = false;
	HideCursor();
	DisableUIMouseInteraction();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
	return m_CursorEnabled;
}

//optional T�� ���� T�� ��ȯ�� ���� �ְ� ����ִ� ��ü�� ��ȯ�� ���� ����.
std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::GetGraphics()
{
	if (pGfx == nullptr)
	{
		throw JHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

void Window::HideCursor() noexcept
{
	//showcursor �Լ��� uint���� ������ showcursor�� �ҷ��������� ++ or --���Ͽ� 0�Ͻ� Ŀ���� ����� 1�̻��� �� Ŀ���� �����ش�
	while (::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor()noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

void Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	//���� �������� Ŭ���̾�Ʈ ��ǥ�� ������� ��ũ�� ��ǥ�� ��ȯ���ش�
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	//Ŀ���� rect�� ������ �� �ְԲ� ��
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::DisableUIMouseInteraction() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_NoMouse;
}

void Window::EnableUIMouseInteraction() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_::ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		//�� CreateWindow�� �Ű������� ������ lParam�� �޾ƿ´� (���� window��ü�� �ּ�)
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//pWnd , HandleMsgThunk �� hWnd�� �Ű������� window���� ���� GWLP_USERDATA , GWLP_WNDPROC���� �ҷ��� �� ����
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


INT_PTR CALLBACK Window::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		hComboHandle = CreateWindowEx(WS_EX_CLIENTEDGE, "Combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 100, 100, 200, 200, hDlg, (HMENU)ID_COMBOBOX, NULL, NULL);
		for (UINT i = 0; i < Adapterdesc.size(); i++)
		{
			size_t size;
			CHAR szString[256];
			wcstombs_s(&size, szString, _countof(szString), (LPCWSTR)Adapterdesc[i].c_str(), Adapterdesc[i].length());
			HRESULT hr = SendMessage(hComboHandle, CB_ADDSTRING, i, (LPARAM)szString);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, SelectIdx);
			break;
		case ID_COMBOBOX:
		{
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				SelectIdx = SendMessage(hComboHandle, CB_GETCURSEL, 0, 0);
				break;
			}
		}
		default:
			break;
		}
	default:
		break;
	}
	return DefWindowProc(hDlg, iMessage, wParam, lParam);
}


INT_PTR CALLBACK Window::HandleDlgProcSetup(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	if (iMessage == WM_INITDIALOG)
	{
		Window* const pWnd = reinterpret_cast<Window*>(lParam);
		//pWnd , HandleMsgThunk �� hWnd�� �Ű������� window���� ���� GWLP_USERDATA , GWLP_WNDPROC���� �ҷ��� �� ����
		SetWindowLongPtr(hDlg, DWLP_USER, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hDlg, DWLP_DLGPROC, reinterpret_cast<LONG_PTR>(&Window::HandleDlgProcThunk));
		return pWnd->MainDlgProc(hDlg, iMessage, wParam, lParam);
	}
	return DefWindowProc(hDlg, iMessage, wParam, lParam);
}

INT_PTR CALLBACK Window::HandleDlgProcThunk(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//window���� ������ window�����͸� �޾ƿͼ� ����Լ��� HandleMsgȣ��!
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hDlg, DWLP_USER));
	return pWnd->MainDlgProc(hDlg, iMessage, wParam, lParam);
}


LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//window���� ������ window�����͸� �޾ƿͼ� ����Լ��� HandleMsgȣ��!
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_CREATE:
		DialogBoxParamA(nullptr, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, HandleDlgProcSetup, (LPARAM)this);
		break;

		//focus�� �Ҿ��� �� Ű�� �ʱ�ȭ��Ű�� ������ ��� �� ���¿� �ӹ�����
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
#pragma region KeyEvent
		//alt, f10���� �ý��� Ű�� �� �޽����� ���� �̺�Ʈ�� �߻�(alt = VK_MENU)
	case WM_SYSKEYDOWN:
		//imgui�� �ý��� Ű�� ĸ�ĸ� ���� ��� �޽��� ���ν����� �ߴܽ�Ų��
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		if (!(lParam & (1 << 30)) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYDOWN:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		if (!(lParam & (1 << 30)) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_SYSKEYUP:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
#pragma endregion
#pragma region MouseEvent
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS(lParam); 
		if (!m_CursorEnabled)
		{
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		
		//Ŭ���̾�Ʈ ���� ���϶�
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			//�ٱ��� �ִٰ� ������
			if (!mouse.IsInWindow())
			{
				//setcapture -> Ŭ���̾�Ʈ ������ ����� ���콺 �̵� �̺�Ʈ�� ���� �� �ִ�.
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		//Ŭ���̾�v ���� �ٱ�
		else
		{
			//��ư�� ���������̸�
			if (wParam &(MK_LBUTTON | MK_RBUTTON))
			{
				//���콺�̵��� ��� ��Ų��
				mouse.OnMouseMove(pt.x, pt.y);
			}
			//��ư�� �� �����̸�
			else
			{
				//ĸ�ĸ� Ǭ��
				ReleaseCapture();
				mouse.OnMouseLeave(); 
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);
		if (!m_CursorEnabled)
		{
			HideCursor();
			ConfineCursor();
		}
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
		{
			break;
		}
		UINT size;
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			break;
		}

		rawBuffer.resize(size);
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) == size)
		{
			break;
		}

		RAWINPUT rawVal = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (rawVal.header.dwType == RIM_TYPEMOUSE &&
			rawVal.data.mouse.lLastX != 0 || rawVal.data.mouse.lLastY != 0)
		{
			mouse.OnRawDelta(rawVal.data.mouse.lLastX, rawVal.data.mouse.lLastY);
		}
		break;
		
	}
#pragma endregion
	case WM_ACTIVATE:
		if (!m_CursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		// defwindowproc���� ���� �ʴ´�, DestoryWindow�� �ι� �θ��� ������ �׳� �Ҹ��ڿ��� ó����
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}


std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	//formatmessage -> hresult�� �޾Ƽ� dec
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);

	if (nMsgLen == 0)
	{
		return "Unidentified Error Code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);

	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "JH Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char * Window::NoGfxException::GetType() const noexcept
{
	return "No Graphics Object Exception";
}
