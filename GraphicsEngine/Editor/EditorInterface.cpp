#include "GraphicsEngine.pch.h"
#include "EditorInterface.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "../GraphicsEngine.h"
#include "../Tools/InputHandler.h"
#include "../Tools/Timer.h"
#include "Editor.h"
#include "../Texture/TextureAssetHandler.h"
#include <commdlg.h>
#include <fstream>

#include "../Particle/ParticleAssetHandler.h"
#include "../Text/TextFactory.h"
#include <UtilityFunctions.hpp>
#include <queue>
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "EC/ComponentRegistry.hpp"
#include <functional>
#include "Scene/SceneHandler.h"

using namespace CommonUtilities;

std::filesystem::path EditorInterface::myCurrentPath = ".\\";

void EditorInterface::ShowEditor()
{
	auto scene = SceneHandler::GetActiveScene();
	//Editor::EditorActionHandler();
	EnableDocking();
	MenuBar();
	//AssetBrowser();
	SceneHierchy(scene);
}

void EditorInterface::EnableDocking()
{
	static bool pOpen = true;
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpacePanel() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpacePanel() active. If a DockSpacePanel() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpacePanel Demo", &pOpen, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpacePanel
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	ImGui::End();
}

void EditorInterface::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
			}
			if (ImGui::MenuItem("Open Scene"))
			{
				OPENFILENAMEA ofn;
				char szFile[260] = { 0 };

				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = GraphicsEngine::GetWindowHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Scene (.scene)\0*.scene\0";
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetOpenFileNameA(&ofn) == TRUE)
				{
					std::filesystem::path fileName(ofn.lpstrFile);
					std::string name = fileName.filename().string();

					name = name.erase(name.size() - 5);
					//SceneHandler::LoadScene(name);
				}
			}
			if (ImGui::MenuItem("Save Scene"))
			{
			}
			ImGui::EndMenu();
		}
		static bool performanceWindow = false;
		if (ImGui::BeginMenu("Performance"))
		{
			performanceWindow = true;
			ImGui::EndMenu();
		}
		if (performanceWindow)
		{
			ImGui::Begin("Performance", &performanceWindow);
			ImGui::Text("Delta Time %.3f", ImGui::GetIO().DeltaTime);
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorInterface::Properties(std::shared_ptr<Entity> aEntity)
{
	ImGui::Begin("Properties");

	if (!aEntity)
	{
		ImGui::End();
		return;
	}

	ImGui::InputText("##", &aEntity->GetName());

	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& trans = aEntity->GetTransform();
			ImGui::DragFloat3("Pos", &trans.GetPositionMutable().x, 1);
			ImGui::DragFloat3("Rot", &trans.GetRotationMutable().x, 1);
			ImGui::DragFloat3("Scale", &trans.GetScaleMutable().x, 1);
			trans.ComposeMatrix();
		}
	}

	for (auto& comp : aEntity->GetComponents())
	{
		comp->Update();
		if (ImGui::CollapsingHeader(comp->GetName().c_str(), ImGuiTreeNodeFlags_Selected))
		{
			auto& variables = comp->GetVariables();
			for (auto& var : variables)
			{
				switch (var.Type)
				{
				case ParameterType::Bool:
					ImGui::Checkbox(var.Name.c_str(), reinterpret_cast<bool*>(var.Value));
					break;
				case ParameterType::Float:
					ImGui::DragFloat(var.Name.c_str(), reinterpret_cast<float*>(var.Value), var.Increment, var.Min, var.Max);
					break;
				case ParameterType::Int:
					ImGui::DragInt(var.Name.c_str(), reinterpret_cast<int*>(var.Value), var.Increment, var.Min, var.Max);
					break;
				case ParameterType::Vec3:
					ImGui::DragFloat3(var.Name.c_str(), &(*reinterpret_cast<Vector3f*>(var.Value)).x, var.Increment, var.Min, var.Max);
					break;
				case ParameterType::String:
					ImGui::InputText(var.Name.c_str(), reinterpret_cast<std::string*>(var.Value));
					break;
				case ParameterType::Button:
					if (ImGui::Button(var.Name.c_str()))
					{
						var.ButtonFunc();
					}
					break;
				}
			}
		}
	}

	ImGui::Button("Add Component");

	ImGui::SetNextWindowSize({ ImGui::GetWindowWidth(), 300 });
	if (ImGui::BeginPopupContextItem(std::to_string(aEntity->GetID()).c_str(), ImGuiPopupFlags_MouseButtonLeft))
	{
		auto& compRegMap = ComponentRegistry::GetFactoryMap();

		// Sort component map when adding component
		std::vector<std::string> keys;
		keys.reserve(compRegMap.size());
		{
			for (auto it = compRegMap.begin(); it != compRegMap.end(); ++it)
			{
				keys.push_back(it->first);
			}
			std::sort(keys.begin(), keys.end(), [&](auto first, auto second) {return first < second; });
		}

		ImGui::BeginChild("##InspectorAddComponentChild");
		for (std::string& key : keys)
		{
			if (ImGui::MenuItem(key.c_str()))
			{
				aEntity->AddComponent(compRegMap[key]());
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}

	ImGui::End();
}

void EditorInterface::SceneHierchy(std::shared_ptr<Scene> aScene)
{
	ImGui::Begin("Scene Hierchy");

	for (auto& entity : aScene->GetEntities())
	{
		if (ImGui::Selectable(entity->GetName().c_str(), (selectedEntity && selectedEntity->GetID() == entity->GetID()) ? true : false))
		{
			selectedEntity = entity;
		}
	}
	if (ImGui::BeginPopupContextWindow("HierchyOptions", ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Entity"))
		{
			aScene->AddEntity();
		}
		ImGui::EndPopup();
	}

	Properties(selectedEntity);

	ImGui::End();

	// SHOULD BE MOVED
	ImGui::Begin("ViewPort");
	Vector2f windowWidth(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::Image((void*)GBuffer::GetVPSRV().Get(), { windowWidth.x, windowWidth.y });
	ImGuiViewport viewport;
	viewPortPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
	viewPortSize = windowWidth;
	if (true)
	{
		//EditorGuizmo(entity);
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets"))
		{
			std::string item = (const char*)payload->Data;
			std::filesystem::path items = item;
			GraphicsEngine::AddAssets(items);
		}
		ImGui::EndDragDropTarget();
	}


	ImGui::End();
}

bool EditorInterface::IsInsideViewPort(Vector2f aPos)
{
	if (viewPortPos.x <= aPos.x && viewPortPos.x + viewPortSize.x >= aPos.x)
	{
		if (viewPortPos.y <= aPos.y && viewPortPos.y + viewPortSize.y >= aPos.y)
		{
			return true;
		}
	}
	return false;
}

bool EditorInterface::IsInsideAssetBrowser(Vector2f aPos)
{
	if (assetBrowserPos.x <= aPos.x && assetBrowserPos.x + assetBrowserSize.x >= aPos.x)
	{
		if (assetBrowserPos.y <= aPos.y && assetBrowserPos.y + assetBrowserSize.y >= aPos.y)
		{
			return true;
		}
	}
	return false;
}

void EditorInterface::DragAndDropHierchy(const int& aIndex)
{
	aIndex;
	//std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	//std::shared_ptr<SceneObject> object;// = scene->GetSceneObjects()[aIndex];
	//std::filesystem::path objName = object->GetName();
	//static std::vector<std::shared_ptr<SceneObject>> SelectedObjects;
	//
	//static bool acceptedDragDrop = false;
	//
	//if (!InputHandler::GetKeyIsHeld('M'))
	//{
	//	if (SelectedObjects.size() > 0)
	//	{
	//		std::cout << "Cleared " << std::endl;
	//		SelectedObjects.clear();
	//	}
	//}
	//if (ImGui::BeginDragDropTarget())
	//{
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
	//	{
	//		if (!acceptedDragDrop)
	//		{
	//			acceptedDragDrop = true;
	//			std::vector<std::shared_ptr<SceneObject>> child = *(std::vector<std::shared_ptr<SceneObject>>*)payload->Data;
	//			for (size_t i = 0; i < child.size(); i++)
	//			{
	//				if (child[i]->myParent)
	//					if (child[i]->myParent->myChildren.size() > 0)
	//						child[i]->myParent->myChildren.erase(std::remove(child[i]->myParent->myChildren.begin(),
	//							child[i]->myParent->myChildren.end(), child[i]));
	//				child[i]->myParent = nullptr;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		acceptedDragDrop = false;
	//	}
	//	ImGui::EndDragDropTarget();
	//}

	//if (ImGui::TreeNodeEx(&object->GetId(), ImGuiTreeNodeFlags_None, objName.string().c_str()))
	//{
	//	if (InputHandler::GetKeyIsHeld('M'))
	//		if (ImGui::IsItemClicked())
	//		{
	//			std::cout << "Added Object: " << objName.string() << std::endl;
	//			SelectedObjects.push_back(object);
	//		}
	//	DragAndDrop(object, SelectedObjects, acceptedDragDrop);
	//	selectedEntity = object->myEntity;
	//	selectedItem = aIndex;
	//	ShowObjectChildren(object, SelectedObjects, acceptedDragDrop);
	//	ImGui::TreePop();
	//	someSelected = true;
	//}
}

void EditorInterface::DragAndDrop(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop)
{
	if (ImGui::BeginDragDropSource())
	{
		if (!InputHandler::GetKeyIsHeld('M'))
			aObjectVector.push_back(aObject);
		ImGui::SetDragDropPayload("ENTITY", &aObjectVector,
			sizeof(std::vector<std::shared_ptr<SceneObject>>));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
		{
			//if (!aAcceptedDragDrop)
			//{
			//	aAcceptedDragDrop = true;
			//	std::vector<std::shared_ptr<SceneObject>> child = *(std::vector<std::shared_ptr<SceneObject>>*)payload->Data;
			//	auto parent = aObject;
			//	for (size_t i = 0; i < child.size(); i++)
			//	{
			//		if (!HasConnection(child[i], parent))
			//		{
			//			if (child[i]->myParent)
			//				if (child[i]->myParent->myChildren.size() > 0)
			//				{
			//					child[i]->myParent->myChildren.erase(std::remove(child[i]->myParent->myChildren.begin(),
			//						child[i]->myParent->myChildren.end(), child[i]), child[i]->myParent->myChildren.end());
			//				}
			//			parent->myChildren.push_back(child[i]);
			//			child[i]->myParent = parent;
			//		}
			//	}
			//}
		}
		else
		{
			aAcceptedDragDrop = false;
		}
		ImGui::EndDragDropTarget();
	}
}

void EditorInterface::AssetBrowser()
{
	ImGui::Begin("Asset Browser");

	static std::vector<std::filesystem::path> currentFiles;
	currentFiles.clear();
	auto windowSize = ImGui::GetWindowSize();
	int count = static_cast<int>(std::floor(windowSize.x / 100));

	assetBrowserPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
	assetBrowserSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	std::shared_ptr<Texture> backArrow;
	if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_back.dds"))
	{
		backArrow = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_back.dds");
	}
	if (ImGui::ImageButton(backArrow->GetSRV().Get(), ImVec2(20, 20)) && myCurrentPath != ".\\")
	{
		myCurrentPath._Remove_filename_and_separator();
		myCurrentPath.remove_filename();
	}
	ImGui::SameLine();
	ImGui::Text(myCurrentPath.string().c_str());

	if (ImGui::BeginTable("##ContentBrowserTable", count, 0))
	{
		for (const auto& file : directory_iterator(myCurrentPath))
		{
			std::filesystem::path currentItem = file;
			std::shared_ptr<Texture> currentIcon;
			if (file.is_directory())
			{
				if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_directory.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_directory.dds");
				}
			}
			else if (currentItem.extension() == ".dds")
			{
				if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_texture.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_texture.dds");
				}
			}
			else if (currentItem.extension() == ".fbx")
			{
				if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_mesh.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_mesh.dds");
				}
			}
			else if (currentItem.extension() == ".cso")
			{
				if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_material.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_material.dds");
				}
			}
			else if (currentItem.extension() == ".json" || currentItem.extension() == ".scene")
			{
				if (TextureAssetHandler::LoadTexture(".\\Editor\\Icons\\icon_file.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(".\\Editor\\Icons\\icon_file.dds");
				}
			}
			else
				continue;
			ImGui::TableNextColumn();

			ImGui::ImageButton(currentIcon->GetSRV().Get(), ImVec2(100, 100));

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && file.is_directory())
			{
				myCurrentPath += currentItem.filename();
				myCurrentPath += "\\";
			}

			currentFiles.push_back(currentItem);


			if (InputHandler::GetMouseOneIsHeld() && !file.is_directory() && ImGui::IsItemHovered())
			{
				if (ImGui::BeginDragDropSource())
				{
					std::string item = currentItem.string();
					ImGui::SetDragDropPayload("Assets", item.c_str(),
						item.size() + 1);
					ImGui::EndDragDropSource();
				}
			}
			ImGui::Text(currentItem.filename().string().c_str());
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

bool EditorInterface::EditorGuizmo(std::shared_ptr<Entity> aEntity)
{
	aEntity;
	//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	//ImGuizmo::SetOrthographic(false);
	//ImGuizmo::SetDrawlist();
	//ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	//std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	//float view[16] = { 0 };
	//float projection[16] = { 0 };
	//float objectMatrix[16] = { 0 };
	//Matrix4x4f viewMatrix = Matrix4x4f::GetFastInverse(scene->GetCamera()->GetTransform().GetMatrix());
	//memcpy(&view[0], &viewMatrix, sizeof(float) * 16);
	//memcpy(&projection[0], &scene->GetCamera()->GetProjection(), sizeof(float) * 16);
	//memcpy(&objectMatrix[0], &scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetMatrix(), sizeof(float) * 16);
	//static int action = 0;
	//static bool change = false;
	//if (InputHandler::GetKeyIsPressed('Q'))
	//	action = 0;
	//if (InputHandler::GetKeyIsPressed('E'))
	//	action = 1;
	//if (InputHandler::GetKeyIsPressed('R'))
	//	action = 2;
	//Transform test;
	//if (action == 0)
	//	if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &objectMatrix[0]))
	//	{
	//		change = true;
	//		//SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetPosition({ objectMatrix[12],objectMatrix[13], objectMatrix[14] });
	//		//ImGui::PopStyleVar();
	//		//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
	//		//return true;
	//	}
	//if (action == 1)
	//	if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::ROTATE, ImGuizmo::WORLD, &objectMatrix[0]))
	//	{
	//		change = true;
	//		//SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetRotation({ objectMatrix[12],objectMatrix[13], objectMatrix[14] });
	//		//ImGui::PopStyleVar();
	//		//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
	//		//return true;
	//	}
	//if (action == 2)
	//	if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::SCALE, ImGuizmo::WORLD, &objectMatrix[0]))
	//	{
	//		change = true;
	//		//ImGui::PopStyleVar();
	//		//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
	//		//return true;
	//	}
	//if (change)
	//{
	//	memcpy(&test.GetMatrix(), &objectMatrix[0], sizeof(float) * 16);
	//	Vector3f pos, rot, scale;
	//	test.GetMatrix().Deconstruct(pos, rot, scale);
	//	switch (action)
	//	{
	//	case 0:
	//		scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetPosition(pos);
	//		break;
	//	case 1:
	//		scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetRotation(rot);
	//		break;
	//	case 2:
	//		scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetScale(scale);
	//		break;
	//	}
	//	ImGui::PopStyleVar();
	//	change = false;
	//	return true;
	//}
	//ImGui::PopStyleVar();

	return false;
}