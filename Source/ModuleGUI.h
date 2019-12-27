#pragma once
#include "Module.h"
#include "UI_Element.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	//bool Init(Config* config = nullptr);
	//bool Start(Config* config = nullptr);
	//bool CleanUp();
	//
	//void Draw();

//private:
	//bool CheckMousePos(UI_Element* data);
	//bool CheckClick(UI_Element* data);

};

