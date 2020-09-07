#pragma once
#include "Window.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Camera.h"
#include "Mesh.h"
#include "PointLight.h"
#include "TestPlane.h"
#include <set>
#include <optional>
class Box;

class App
{
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
	void SpawnWindow();
private:
	ImguiManager imguimanager;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight Light;
	Model plane{ wnd.GetGraphics(), "Models\\brick_wall.obj" };
	float speedFactors = 1.0f;
private:
};