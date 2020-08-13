#pragma once
#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "SkinnedBox.h"
#include "Pyramid.h"
#include <optional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "JHMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"
GDIPlusManager gdimanager;

App::App() : wnd(1280,720,"JH's Direct3D"), Light(wnd.GetGraphics())
{
	wnd.GetGraphics().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 1000.0f));
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
	const DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	//pixel cb register1
	nano.Draw(wnd.GetGraphics(), transform);

	Light.Draw(wnd.GetGraphics());

#if IS_DEBUG
	static char buffer[1024];
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speedFactors, 0.1f, 3.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate , ImGui::GetIO().Framerate);
	}
	ImGui::End();


	cam.SpawnControlWindow();
	Light.SpawnControlWindow();
	SpawnWindow();
#endif
	wnd.GetGraphics().EndFrame();
}

void App::SpawnWindow()
{
	if(ImGui::Begin("Model"))
	{
		ImGui::Text("Rotation");
		ImGui::SliderAngle("Roll",&pos.roll,-180.0f,180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Translation");
		ImGui::SliderFloat("X", &pos.x , -20.0f , 20.0f);
		ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}
