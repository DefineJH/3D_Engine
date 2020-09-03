#pragma once
#include "App.h"
#include <optional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "JHMath.h"
#include "GDIPlusManager.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"
GDIPlusManager gdimanager;

App::App() : wnd(1280,720,"JH's Direct3D"), Light(wnd.GetGraphics())
{
	wnd.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(wnd.GetHeight()) / wnd.GetWidth(), 0.5f, 1000.0f));
}
App::~App()
{}

int App::Go()
{
	while (true)
	{
		// optional<int> ecode 는 wm_quit일 때 msg.wParam을 반환받으므로 true
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	auto dt = timer.Mark() * speedFactors;
	wnd.GetGraphics().BeginFrame(0, 0, 0);
	wnd.GetGraphics().SetCamera(cam.GetMatrix());
	//pixel cb register0
	Light.Bind(wnd.GetGraphics(), wnd.GetGraphics().GetCamera());
	//pixel cb register1
	//nano.Draw(wnd.GetGraphics());
	//nano2.Draw(wnd.GetGraphics());
	plane.Draw(wnd.GetGraphics());
	Light.Draw(wnd.GetGraphics());
	while (!wnd.kbd.KeyIsEmpty())
	{
		const auto e = wnd.kbd.ReadKey();
		if (!e.IsPress())
		{
			continue;
		}
		switch (e.GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		}
	}
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}

#if IS_DEBUG
	static char buffer[1024];
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speedFactors, 0.1f, 3.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate , ImGui::GetIO().Framerate);
	}
	ImGui::End();

	nano.ShowWindow("Nano");
	nano2.ShowWindow("Nano2");
	cam.SpawnControlWindow();
	Light.SpawnControlWindow();
	SpawnWindow();
#endif
	wnd.GetGraphics().EndFrame();
}

void App::SpawnWindow()
{
	if(ImGui::Begin("Debugging"))
	{
		ImGui::Text("Cursor %s", wnd.CursorEnabled() ? "enabled" : "Disabled");
	}
	ImGui::End();
}
