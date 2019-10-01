#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	bool Start();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool CleanUp();

	void Draw() const;
	void CreateLink(const char* text, const char* url, bool bullet = false);

};

#endif
