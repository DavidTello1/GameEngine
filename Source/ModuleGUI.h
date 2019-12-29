#pragma once
#include "Module.h"
#include "UI_Element.h"
#include "Canvas.h"
#include <vector>

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Init(Config* config = nullptr);
	bool Start(Config* config = nullptr);
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	
	void Draw(ComponentCamera * camera);
	void AddCanvas(Canvas* c) { canvas.push_back(c); }
	
public:
	std::vector<Canvas*> canvas;
};

