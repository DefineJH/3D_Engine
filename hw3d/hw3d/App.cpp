#pragma once
#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "SkinnedBox.h"
#include "Tank.h"
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
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };

			switch (sdist(rng))
			{
			case 0:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist, mat
					);
			case 1:
				return std::make_unique<Cylinder>(
					gfx, rng, adist, ddist, odist,
					rdist,bdist ,tdist
					);
			case 2:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist,
					rdist, tdist
					);
			case 3:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<Tank>
					(gfx, rng, adist, ddist,
						odist, rdist, bdist, mat);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
			/*
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}*/
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0,4 };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
		std::uniform_int_distribution<int> tdist{ 3,30 };
	};

	Factory f(wnd.GetGraphics());
	drawables.reserve(nDrawable);
	std::generate_n(std::back_inserter(drawables), nDrawable, f);

	for (auto i = 0; i < drawables.size(); i++)
	{
		Box * boxptr = dynamic_cast<Box*>(drawables[i].get());
		if (boxptr != nullptr)
		{
			boxes.push_back(boxptr);
		}
	}
	
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

	Light.Bind(wnd.GetGraphics(),wnd.GetGraphics().GetCamera());
	for (auto& b : drawables)
	{
		b->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		b->Draw(wnd.GetGraphics());
	}
	Light.Draw(wnd.GetGraphics());

#if IS_DEBUG
	static char buffer[1024];
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speedFactors, 0.1f, 3.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate , ImGui::GetIO().Framerate);
	}
	ImGui::End();

	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = *comboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
	for (auto id : boxControlIds)
	{
		boxes[id]->SpawnBoxControllWindow(wnd.GetGraphics());
	}

	cam.SpawnControlWindow();
	Light.SpawnControlWindow();
#endif
	wnd.GetGraphics().EndFrame();
}

void App::SpawnWindow()
{

}
