#pragma once
#include "Window.h"
#include "ImguiManager.h"
#include "Timer.h"
#include "Camera.h"
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
	std::vector<Box*> boxes;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	float speedFactors = 1.0f;
	static constexpr size_t nDrawable = 180;
};