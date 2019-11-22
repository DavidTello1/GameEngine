#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

// ---------------------------------------------------------
Hierarchy::Hierarchy() : Panel("Hierarchy")
{
	width = default_width;
	height = default_height;
	pos_x = default_pos_x;
	pos_y = default_pos_y;

	has_menubar = true;
}

Hierarchy::~Hierarchy()
{}

void Hierarchy::Draw()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
			DrawCreateMenu();

		if (ImGui::MenuItem("Delete") || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
			App->scene->DeleteSelected();

		if (ImGui::MenuItem("Rename", NULL, false, App->scene->GetSelectedGameObject() != nullptr))
			App->scene->GetSelectedGameObject()->is_rename = true;

		ImGui::EndMenuBar();
	}

	ImGui::Text(App->scene->GetName());
	ImGui::Separator();

	// Drawing scene gameobjects as nodes
	DrawNodes(ModuleScene::root_object->childs);
}

void Hierarchy::DrawNodes(std::vector<GameObject*>& v)
{
	for (GameObject* obj : v)
	{
		if (obj->is_rename == false)
		{
			bool is_open = ImGui::TreeNodeEx(obj->GetName(), obj->hierarchy_flags);

			if (ImGui::IsItemClicked()) // if treenode is clicked, check whether it is a single or multi selection
			{
				if (ImGui::IsMouseDoubleClicked(0)) // Double Click
				{
					obj->is_rename = true;
				}
				else 
				{
					if (!ImGui::GetIO().KeyCtrl) // Single selection, clear selected nodes
					{
						App->scene->UnSelectAll(obj);
					}
					
					obj->ToggleSelection(); // Always need to toggle the state of selection of the node, getting its current state
				}
			}

			if (is_open)
			{
				// Node is open, need to draw childs if has childs
				if (obj->HasChilds()) 
					DrawNodes(obj->childs);

				ImGui::TreePop();
			}
		}
		else // Rename
		{
			if (obj->is_selected == false)
			{
				obj->is_rename = false;
			}

			sprintf_s(buffer, NAME_LENGTH, "%s", obj->GetName());
			if (ImGui::InputText("##GameObject", buffer, NAME_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				obj->SetName(buffer);
				obj->is_rename = false;
			}
		}
	}
}

void Hierarchy::DrawCreateMenu()
{
	if (ImGui::MenuItem("Empty"))
		App->scene->CreateGameObject();

	GameObject* parent = App->scene->GetSelectedGameObject();

	ImGui::Separator();
	if (ImGui::BeginMenu("Basic shapes"))
	{
		//if (ImGui::MenuItem("Cylinder"))
		//	App->resources->CreateShape(CYLINDER, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Cone"))
		//	App->resources->CreateShape(CONE, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Sphere"))
		//	App->resources->CreateShape(SPHERE, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Plane"))
		//	App->resources->CreateShape(PLANE, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Cube"))
		//	App->resources->CreateShape(CUBE, 9, 9,0,0,0,0.5f, parent);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Extended shapes"))
	{
		//if (ImGui::MenuItem("Torus"))
		//	App->resources->CreateShape(TORUS, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Bottle"))
		//	App->resources->CreateShape(BOTTLE, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Knot"))
		//	App->resources->CreateShape(KNOT, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Hemisphere"))
		//	App->resources->CreateShape(HEMISPHERE, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Icosahedron"))
		//	App->resources->CreateShape(ICOSAHEDRON, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Dodecahedron"))
		//	App->resources->CreateShape(DODECAHEDRON, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Octahedron"))
		//	App->resources->CreateShape(OCTAHEDRON, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Tetrahedron"))
		//	App->resources->CreateShape(TETRAHEDRON, 9, 9,0,0,0,0.5f, parent);

		//if (ImGui::MenuItem("Rock"))
		//	App->resources->CreateShape(ROCK, 9, 9,0,0,0,0.5f, parent);

		ImGui::EndMenu();
	}
	ImGui::EndMenu();
}