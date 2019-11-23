#include "Application.h"
#include "Hierarchy.h"
#include "Globals.h"
#include "ResourceMesh.h"

#include "ModuleScene.h"
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
		if (ImGui::MenuItem("Cylinder"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, CYLINDER, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Cone"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, CONE, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, SPHERE, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Plane"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, PLANE, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Cube"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, CUBE, 9, 9, 0, 0, 0, 0.5f);
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Extended shapes"))
	{
		if (ImGui::MenuItem("Torus"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, TORUS, 9, 9, 0, 0, 0, 0.5f);
		}			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
		if (ImGui::MenuItem("Bottle"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, BOTTLE, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Knot"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, KNOT, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Hemisphere"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, HEMISPHERE, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Icosahedron"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, ICOSAHEDRON, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Dodecahedron"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, DODECAHEDRON, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Octahedron"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, OCTAHEDRON, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Tetrahedron"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, TETRAHEDRON, 9, 9, 0, 0, 0, 0.5f);
		}
		if (ImGui::MenuItem("Rock"))
		{
			ResourceMesh* res = static_cast<ResourceMesh*>(App->resources->CreateResource(Resource::mesh));
			res->CreateShape(res, ROCK, 9, 9, 0, 0, 0, 0.5f);
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenu();
}