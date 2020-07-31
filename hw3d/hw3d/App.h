#pragma once
#include "Window.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Camera.h"
#include "PointLight.h"
class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imguimanager;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight Light;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	float speedFactor = 1.0f;
	static constexpr size_t nDrawable = 180;
};