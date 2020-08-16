#pragma once
#include "ChiliWin.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "JHException.h"
#include "Graphics.h"
#include "ImGUI/imgui_impl_win32.h"
#include <optional>
#include <memory>


class Window
{
public:
	class Exception : public JHException
	{
		using JHException::JHException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
private:
	//실제 윈도우의 등록과 해제를 싱글턴에서 담당한다
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "JH's Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string title);
	int GetWidth() const noexcept { return width; }
	int GetHeight() const noexcept { return height; }
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	//모든 윈도우 객체에서 같은 메서드 사용
	static std::optional<int> ProcessMessages();
	Graphics& GetGraphics();
private:
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	//모니터의 좌표를 현재 클라이언트 공간에 매핑하여 커서가 윈도우 밖으로 나갈수 없게끔 한정(confine)함
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	void DisableUIMouseInteraction() noexcept;
	void EnableUIMouseInteraction() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// 멤버함수로 메시지를 처리하기 위해 위 두개의 static 함수를 두고 우회한다.
	// 왜냐하면 기본적으로 멤버함수는 객체의 포인터를 숨겨진 매개변수로 받는데, 윈도우 사이드는 이를 모르기 때문에
	// 객체에 상대적이지 않은 static 함수를 이용하여 우회한다
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	bool m_CursorEnabled = true;
public:
	Keyboard kbd;
	Mouse mouse;


};
