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
	//���� �������� ��ϰ� ������ �̱��Ͽ��� ����Ѵ�
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
	//��� ������ ��ü���� ���� �޼��� ���
	static std::optional<int> ProcessMessages();
	Graphics& GetGraphics();
private:
	void HideCursor() noexcept;
	void ShowCursor() noexcept;
	//������� ��ǥ�� ���� Ŭ���̾�Ʈ ������ �����Ͽ� Ŀ���� ������ ������ ������ ���Բ� ����(confine)��
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;

	void DisableUIMouseInteraction() noexcept;
	void EnableUIMouseInteraction() noexcept;
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// ����Լ��� �޽����� ó���ϱ� ���� �� �ΰ��� static �Լ��� �ΰ� ��ȸ�Ѵ�.
	// �ֳ��ϸ� �⺻������ ����Լ��� ��ü�� �����͸� ������ �Ű������� �޴µ�, ������ ���̵�� �̸� �𸣱� ������
	// ��ü�� ��������� ���� static �Լ��� �̿��Ͽ� ��ȸ�Ѵ�
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
