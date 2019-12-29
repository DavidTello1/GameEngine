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
	void Draw();
	bool CleanUp();


	void AddCanvas(Canvas* c);
	
	std::vector<Canvas*> canvas;
};

