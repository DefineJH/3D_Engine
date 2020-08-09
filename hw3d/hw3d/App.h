#pragma once
#include "Window.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Camera.h"
#include "Model.h"
#include "PointLight.h"
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
	Model nano{ wnd.GetGraphics(), "Models\\nanosuit.obj" };
	float speedFactors = 1.0f;
private:
	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	}pos;
};