#include "GraphicsEngine.pch.h"
#include "EditorInterface.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include "../GraphicsEngine.h"
#include "../Tools/InputHandler.h"
#include "../Tools/Timer.h"
#include "Editor.h"
#include "../Scene/SceneHandler.h"
#include "../Texture/TextureAssetHandler.h"
#include <commdlg.h>
#include <fstream>

#include "../Engine/ComponentHandler.h"
#include "../Particle/ParticleAssetHandler.h"
#include "../Text/TextFactory.h"
#include <UtilityFunctions.hpp>
#include <queue>

using namespace CommonUtilities;

bool EditorInterface::addAnimation;
bool EditorInterface::someSelected;
int EditorInterface::selectedItem;
entt::entity EditorInterface::selectedEntity;
std::filesystem::path EditorInterface::myCurrentPath = ".\\";

void EditorInterface::ShowEditor()
{
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	Editor::EditorActionHandler();
	EnableDocking();
	MenuBar();
	ModelLoader();
	AssetBrowser();
	SceneHierchy(scene);
}

void EditorInterface::SetTexture(std::wstring aFilePath)
{
	if (selectedItem >= 0)
	{
		std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
		if (scene->GetRegistry().all_of<ModelComponent>(selectedEntity))
		{
			std::shared_ptr<ModelInstance> mdl = scene->GetRegistry().get<ModelComponent>(selectedEntity).myModel;
			std::filesystem::path path = aFilePath;
			ModelAssetHandler::SetModelTexture(mdl, path.replace_extension(""));
		}
	}
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
}

void EditorInterface::MenuBar()
{
	ImGui::ShowDemoWindow();
	if (ImGui::BeginMainMenuBar())
	{
		static bool createScene = false;
		static bool openedSettings = false;
		if (createScene)
		{
			ImGui::OpenPopup("Scene");
			if (ImGui::BeginPopup("Scene"))
			{
				char curValue[256];
				static char prevValue[256];
				memset(curValue, 0, sizeof(curValue));
				memcpy(curValue, prevValue, sizeof(prevValue));

				if (ImGui::InputText("Scene Name", curValue, sizeof(curValue)))
				{
					memcpy(prevValue, curValue, sizeof(curValue));
				}
				std::filesystem::path name(prevValue);
				if (InputHandler::GetKeyIsPressed(VK_RETURN))
				{
					std::shared_ptr<Scene> scene = SceneHandler::AddEmptyScene(name);
					SceneHandler::LoadScene(scene);
					std::ofstream fileName("Json/Scenes/" + name.string() + ".scene");

					using nlohmann::json;
					json j;
					j["SceneName"] = name.string();
					j["Size"] = 0;
					std::ofstream oStream("Json/Scenes/" + name.string() + ".scene");
					oStream << j;
					createScene = false;
				}
				if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
				{
					createScene = false;
				}

				ImGui::EndPopup();
			}
		}

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene"))
			{
				createScene = true;
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
					std::wstring name = fileName.filename();

					name = name.erase(name.size() - 5);
					SceneHandler::LoadScene(name);
				}
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				Editor::SaveCurrentScene();
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
		if (ImGui::BeginMenu("Game Objects"))
		{
			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
					std::shared_ptr<ModelInstance> mdl = ModelAssetHandler::CreateCube(L"Cube");
					SceneHandler::GetActiveScene()->AddModelInstance(mdl, entity);
					Editor::EditorActions action;
					action.AddedObject = true;
					action.Object = mdl;
					action.oldEntity = entity;
					Editor::AddUndoAction(action);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Particle System"))
			{
				entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
				EmitterSettingsData data;
				std::shared_ptr<ParticleSystem> system = ParticleAssetHandler::CreateParticleSystem(data);
				Editor::EditorActions action;
				action.AddedObject = true;
				action.Object = system;
				action.oldEntity = entity;
				Editor::AddUndoAction(action);
				SceneHandler::GetActiveScene()->AddParticleSystem(system, entity);
			}
			if (ImGui::MenuItem("Text Element"))
			{
				entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
				std::shared_ptr<Text> text = TextFactory::CreateText(L"Text", 1, 12, true);
				SceneHandler::GetActiveScene()->AddText(text, entity);
				Editor::EditorActions action;
				action.AddedObject = true;
				action.Object = text;
				action.oldEntity = entity;
				Editor::AddUndoAction(action);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			openedSettings = true;
			ImGui::EndMenu();
		}
		if (openedSettings)
		{
			ImGui::Begin("Settings", &openedSettings);

			ImGui::DragFloat("Camera Speed", &SceneHandler::GetActiveScene()->GetCamera()->GetCameraSpeed(), 0.1f, 0, 999.f);

			float timeScale = CommonUtilities::Timer::GetTimeScale();
			ImGui::DragFloat("Time Scale", &timeScale, 0.05f, 0, 999.f);
			CommonUtilities::Timer::SetTimeScale(timeScale);
			if (!GraphicsEngine::myClearColorBlending)
			{
				std::array<FLOAT, 4> color = GraphicsEngine::GetClearColor();
				if (ImGui::ColorEdit4("Clear Color", &color[0]))
				{
					GraphicsEngine::GetClearColor() = color;
				}
			}
			static bool myPreset1Selected = true;
			if (ImGui::Button("Use Preset 1"))
			{
				GraphicsEngine::GetClearColor() = GraphicsEngine::myClearColorPresets[0];
				GraphicsEngine::GetClearColor()[3] = 1;
			}
			ImVec4 col = { GraphicsEngine::myClearColorPresets[0][0], GraphicsEngine::myClearColorPresets[0][1], GraphicsEngine::myClearColorPresets[0][2],
				GraphicsEngine::myClearColorPresets[0][3] };
			ImGui::SameLine();
			ImGui::ColorButton("Color 1", col);
			ImGui::SameLine();
			if (ImGui::Button("Use Preset 2"))
			{
				GraphicsEngine::GetClearColor() = GraphicsEngine::myClearColorPresets[1];
				GraphicsEngine::GetClearColor()[3] = 1;
			}
			ImGui::SameLine();
			col = { GraphicsEngine::myClearColorPresets[1][0], GraphicsEngine::myClearColorPresets[1][1], GraphicsEngine::myClearColorPresets[1][2],
				GraphicsEngine::myClearColorPresets[1][3] };
			ImGui::ColorButton("Color 2", col);
			ImGui::SameLine();
			ImGui::Checkbox("Blend", &GraphicsEngine::myClearColorBlending);
			if (GraphicsEngine::myClearColorBlending)
			{
				CommonUtilities::Vector4<float> start;
				memcpy(&start, &GraphicsEngine::myClearColorPresets[0], sizeof(Vector4f));
				CommonUtilities::Vector4<float> end;
				memcpy(&end, &GraphicsEngine::myClearColorPresets[1], sizeof(Vector4f));
				ImGui::DragFloat("Blend Factor", &GraphicsEngine::myClearColorBlendFactor, 0.005f, 0.0f, 1.0f);
				Vector4f value = CommonUtilities::Lerp(start, end, GraphicsEngine::myClearColorBlendFactor);
				memcpy(&GraphicsEngine::GetClearColor(), &value, sizeof(Vector4f));
			}
			static bool save = false;
			if (ImGui::Button("Save Preset"))
			{
				save = true;
			}
			if (save)
			{
				if (ImGui::Button("Save To Preset 1"))
				{
					GraphicsEngine::myClearColorPresets[0] = GraphicsEngine::GetClearColor();
					save = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save To Preset 2"))
				{
					GraphicsEngine::myClearColorPresets[1] = GraphicsEngine::GetClearColor();
					save = false;
				}
			}
			static bool deletePreset = false;
			if (ImGui::Button("Delete Preset"))
			{
				deletePreset = true;
			}
			if (deletePreset)
			{
				char curValue[256];
				static char prevValue[256];
				memset(curValue, 0, sizeof(curValue));
				memcpy(curValue, prevValue, sizeof(prevValue));

				if (ImGui::InputText("Preset Name", curValue, sizeof(curValue)))
				{
					memcpy(prevValue, curValue, sizeof(curValue));
				}
				std::filesystem::path name(prevValue);
				if (InputHandler::GetKeyIsPressed(VK_RETURN))
				{
					if (name.string() != "Settings" && name.string() != "settings")
					{
						std::string fullName = "Json/Settings/" + name.string() + ".json";
						remove(fullName.c_str());
					}
					deletePreset = false;
				}
				if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
				{
					deletePreset = false;
				}
			}
			ImGui::Checkbox("Auto Save", &GraphicsEngine::GetAutoSave());
			ImGui::NewLine();
			static bool saveSettings = false;
			static bool loadSettings = false;
			if (ImGui::Button("Save Settings"))
			{
				Editor::SaveSettings();
				saveSettings = true;
			}
			if (ImGui::Button("Load Preset"))
			{
				loadSettings = true;
			}
			if (saveSettings)
			{
				ImGui::OpenPopup("Preset");
				if (ImGui::BeginPopup("Preset"))
				{
					char curValue[256];
					static char prevValue[256];
					memset(curValue, 0, sizeof(curValue));
					memcpy(curValue, prevValue, sizeof(prevValue));

					if (ImGui::InputText("Preset Name", curValue, sizeof(curValue)))
					{
						memcpy(prevValue, curValue, sizeof(curValue));
					}
					std::filesystem::path name(prevValue);
					if (InputHandler::GetKeyIsPressed(VK_RETURN))
					{
						Editor::SaveClearColorPreset(name.string());
						saveSettings = false;
					}
					if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
					{
						saveSettings = false;
					}

					ImGui::EndPopup();
				}
			}
			if (loadSettings)
			{
				ImGui::OpenPopup("Preset");
				if (ImGui::BeginPopup("Preset"))
				{
					char curValue[256];
					static char prevValue[256];
					memset(curValue, 0, sizeof(curValue));
					memcpy(curValue, prevValue, sizeof(prevValue));

					if (ImGui::InputText("Preset Name", curValue, sizeof(curValue)))
					{
						memcpy(prevValue, curValue, sizeof(curValue));
					}
					std::filesystem::path name(prevValue);
					if (InputHandler::GetKeyIsPressed(VK_RETURN))
					{
						Editor::LoadClearColorPreset(name.string());
						loadSettings = false;
					}
					if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
					{
						loadSettings = false;
					}

					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}
		static bool showUpgift = false;
		if (ImGui::BeginMenu("U4 Read Me"))
		{
			showUpgift = true;
			ImGui::EndMenu();
		}
		if (showUpgift)
		{
			ImGui::Begin("U4");
			ImGui::Text("Tjena! Du kan gora objekten till foralderlost genom att dra de till 'Scene' objektet.");
			ImGui::NewLine();
			ImGui::Text("Nar du haller in 'M' sa har du multi select igang och nar du slapper sa forsvinner det du selectat");
			ImGui::NewLine();
			ImGui::Text("OBS: Objekt laggs bara till nar man klickar sa att de blir selectade sa det gar inte att multiselecta pa ett redan oppet item");
			ImGui::Text("Du kan dock dubbelklicka pa det for att oppna det igen, tror det bara ar sa tree nodes funkar men kan ha fel men jag skiver ut i konsol fonstret nar du lagger till/tar bort for fortydligande");

			//ImGui::Text("Du kan ctrl + z for att angra namn, spawn av objekt och bort tagandet av objekt och du kan undo allt detta");
			//ImGui::NewLine();
			//ImGui::Text("Du kan aven byta ut textur attributerna enskillt genom componentens combo eller genom drag and drop");
			if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
			{
				showUpgift = false;
			}
			ImGui::End();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorInterface::ModelLoader()
{
	ImGui::Begin("Models");
	static std::vector<std::filesystem::path> models;
	static std::vector<std::filesystem::path> animations;
	static int selectedModel = 0;
	static int selectedAnimation = 0;
	static bool gotModels = false;
	static bool selectable = false;
	if (ImGui::Button("Get Models"))
	{
		models.clear();
		animations.clear();
		const std::string modelPath = "./Models";
		for (const auto& file : directory_iterator(modelPath))
		{
			std::string fileName = file.path().filename().string();
			if (fileName.back() == 'x')
			{
				models.push_back(fileName.c_str());
			}
		}
		const std::string animPath = "./Animations";
		for (const auto& file : directory_iterator(animPath))
		{
			std::string fileName = file.path().string();
			if (fileName.back() == 'x')
			{
				animations.push_back(fileName.c_str());
			}
		}
		gotModels = true;
	}
	if (gotModels)
	{
		static std::vector<const char*> charModelVector;
		static std::vector<const char*> charAnimationsVector;
		if (charModelVector.size() != models.size())
		{
			for (size_t i = 0; i < models.size(); i++)
			{
				charModelVector.push_back(models[i].string().c_str());
			}
		}
		if (charAnimationsVector.size() != animations.size())
		{
			for (size_t i = 0; i < animations.size(); i++)
			{
				charAnimationsVector.push_back(animations[i].string().c_str());
			}
		}
		if (ImGui::BeginCombo("Models", models[selectedModel].string().c_str()))
		{
			for (size_t i = 0; i < models.size(); i++)
			{
				if (ImGui::Selectable(models[i].filename().string().c_str(), &selectable))
				{
					selectedModel = static_cast<int>(i);
				}
				if (selectable)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (addAnimation)
		{
			std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
			if (scene->GetRegistry().any_of<ModelComponent>(selectedEntity))
			{
				std::shared_ptr<ModelInstance> mdl = scene->GetRegistry().get<ModelComponent>
					(selectedEntity).myModel;
				if (!mdl->HasBones())
				{
					addAnimation = false;
				}
				if (ImGui::BeginCombo("Animations", animations[selectedAnimation].string().c_str()))
				{
					for (size_t i = 0; i < animations.size(); i++)
					{
						if (ImGui::Selectable(animations[i].filename().string().c_str(), &selectable))
						{
							selectedAnimation = static_cast<int>(i);
						}
						if (selectable)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Add Animation"))
				{
					std::filesystem::path name(animations[selectedAnimation]);
					ModelAssetHandler::LoadAnimation(mdl->GetPath(), name);
					addAnimation = false;
				}
			}
		}
		else
		{
			static bool modelHasBones = false;
			static std::shared_ptr<ModelInstance> model;
			if (ImGui::Button("Load Model"))
			{
				std::filesystem::path name("Models/" + models[selectedModel].string());
				model = ModelAssetHandler::LoadModel(name);
				if (model != nullptr)
				{
					if (!model->HasBones())
					{
						modelHasBones = false;
						entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
						SceneHandler::GetActiveScene()->AddModelInstance(model, entity);
						Editor::EditorActions action;
						action.AddedObject = true;
						action.Object = model;
						action.oldEntity = entity;
						Editor::AddUndoAction(action);
					}
					else if (model->HasBones())
					{
						modelHasBones = true;
					}
				}
			}
			if (modelHasBones)
			{
				if (ImGui::BeginCombo("Animation", animations[selectedAnimation].filename().string().c_str()))
				{
					for (size_t i = 0; i < animations.size(); i++)
					{
						if (ImGui::Selectable(animations[i].filename().string().c_str(), &selectable))
						{
							selectedAnimation = static_cast<int>(i);
						}
						if (selectable)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Load Animation"))
				{
					std::filesystem::path name(animations[selectedAnimation].string());
					ModelAssetHandler::LoadAnimation(model->GetPath(), name);
					model = ModelAssetHandler::GetModelInstance(model->GetPath());
					if (model != nullptr)
					{
						modelHasBones = false;
						entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
						SceneHandler::GetActiveScene()->AddModelInstance(model, entity);
						Editor::EditorActions action;
						action.AddedObject = true;
						action.Object = model;
						Editor::AddUndoAction(action);
					}
				}
			}
		}
	}
	ImGui::End();
}

void EditorInterface::SceneHierchy(std::shared_ptr<Scene> aScene)
{
	ImGui::Begin("Scene Hierchy");
	someSelected = false;
	static bool canChangeName = false;
	if (ImGui::TreeNodeEx("Scene"))
	{
		for (size_t i = 0; i < aScene->GetSceneObjects().size(); i++)
		{
			if (i >= aScene->GetSceneObjects().size())
				continue;
			DragAndDropHierchy(static_cast<int>(i));
			if (someSelected)
			{
				bool deleteItem = false;
				if (i >= aScene->GetSceneObjects().size())
				{
					ImGui::TreePop();
					return;
				}

				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[i]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Change Name"))
					{
						canChangeName = true;
					}
					if (aScene->GetRegistry().any_of<ModelComponent>(selectedEntity))
					{
						if (aScene->GetRegistry().get<ModelComponent>(selectedEntity).myModel->HasBones())
						{
							if (ImGui::MenuItem("Add Animation"))
							{
								addAnimation = true;
							}
						}
					}
					if (ImGui::MenuItem("Destroy"))
					{
						deleteItem = true;
					}
					ImGui::EndPopup();
				}
				if (deleteItem || InputHandler::GetKeyIsPressed(VK_DELETE))
				{
					for (size_t j = 0; j < aScene->GetSceneObjects()[selectedItem]->myChildren.size(); j++)
					{
						Editor::EditorActions action;
						entt::entity ent = aScene->GetSceneObjects()[selectedItem]->myEntity;
						entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
						action.RemovedObject = true;
						action.Object = aScene->GetSceneObjects()[selectedItem];
						action.oldEntity = ent;
						Editor::AddUndoAction(action);

						if (reg.any_of<ModelComponent>(ent))
							aScene->RemoveModelInstance(reg.get<ModelComponent>(ent).myModel);
						if (reg.any_of<ParticleSystemComponent>(ent))
							aScene->RemoveParticleSystem(reg.get<ParticleSystemComponent>(ent).myParticleSystem);
						if (reg.any_of<TextComponent>(ent))
							aScene->RemoveText(reg.get<TextComponent>(ent).myText);
					}
					Editor::EditorActions action;
					entt::entity ent = selectedEntity;
					entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
					action.RemovedObject = true;
					action.Object = aScene->GetSceneObjects()[selectedItem];
					action.oldEntity = ent;
					Editor::AddUndoAction(action);

					if (reg.any_of<ModelComponent>(ent))
						aScene->RemoveModelInstance(reg.get<ModelComponent>(ent).myModel);
					if (reg.any_of<ParticleSystemComponent>(ent))
						aScene->RemoveParticleSystem(reg.get<ParticleSystemComponent>(ent).myParticleSystem);
					if (reg.any_of<TextComponent>(ent))
						aScene->RemoveText(reg.get<TextComponent>(ent).myText);
					someSelected = false;

				}
				static bool stillPressed = false;
				if (!stillPressed)
				{
					if (InputHandler::GetKeyIsPressed('V'))
					{
						Editor::EditorActions action;
						entt::entity entity = ComponentHandler::DuplicateEntity(selectedEntity);
						action.AddedObject = true;
						action.oldEntity = entity;
						entt::registry& reg = aScene->GetRegistry();

						if (reg.all_of<ModelComponent>(entity))
						{
							std::shared_ptr<ModelInstance> mdl;
							if (!reg.get<ModelComponent>(selectedEntity).myModel->HasBones())
							{
								mdl = ModelAssetHandler::LoadModel(reg.get<ModelComponent>(selectedEntity).myModel->GetPath());
							}
							else
							{
								mdl = ModelAssetHandler::LoadModelWithAnimation(reg.get<ModelComponent>(selectedEntity).myModel->GetPath(),
									reg.get<ModelComponent>(selectedEntity).myModel->GetCurrentAnimation().myName);
							}
							reg.get<ModelComponent>(entity).myModel = mdl;
							aScene->AddModelInstance(reg.get<ModelComponent>(entity).myModel, entity);
							action.Object = reg.get<ModelComponent>(entity).myModel;
						}
						if (reg.all_of<TextComponent>(entity))
						{
							std::shared_ptr<Text> text = reg.get<TextComponent>(selectedEntity).myText;
							aScene->AddText(TextFactory::CreateText(text->GetText(), 1, text->GetFont()->Atlas.Size, text->GetIs2D()), entity);
							reg.get<TextComponent>(entity).myText = reg.get<TextComponent>(entity).myText;
							action.Object = reg.get<TextComponent>(entity).myText;
						}
						if (reg.all_of<ParticleSystemComponent>(entity))
						{
							std::shared_ptr<ParticleSystem> system = reg.get<ParticleSystemComponent>(selectedEntity).myParticleSystem;
							aScene->AddParticleSystem(ParticleAssetHandler::CreateParticleSystem(system->GetEmitters()[0].GetEmitterSettings()), entity);
							reg.get<ParticleSystemComponent>(entity).myParticleSystem = reg.get<ParticleSystemComponent>(entity).myParticleSystem;
							action.Object = reg.get<ParticleSystemComponent>(entity).myParticleSystem;
						}
						Editor::AddUndoAction(action);
						stillPressed = true;
					}
				}
				if (InputHandler::GetKeyWasReleased('V'))
				{
					stillPressed = false;
				}
			}
			if (i >= aScene->GetSceneObjects().size() - 1 && someSelected == false)
			{
				selectedItem = -1;
				canChangeName = false;
			}
		}
		ImGui::TreePop();
	}
	if (canChangeName)
	{
		char curValue[256];
		static char prevValue[256];
		memset(curValue, 0, sizeof(curValue));
		memcpy(curValue, prevValue, sizeof(prevValue));

		static std::wstring oldName;
		if (oldName.empty())
			oldName = aScene->GetSceneObjects()[selectedItem]->GetName();

		if (ImGui::InputText("Name", curValue, sizeof(curValue)))
		{
			memcpy(prevValue, curValue, sizeof(curValue));
			std::filesystem::path name(curValue);
			aScene->GetAllSceneObjects()[selectedItem]->SetName(name);
		}
		if (InputHandler::GetKeyIsPressed(VK_RETURN))
		{
			Editor::EditorActions action;
			action.ChangedName = true;
			action.Object = aScene->GetSceneObjects()[selectedItem];
			action.OldName = oldName;
			Editor::AddUndoAction(action);
			canChangeName = false;
			oldName = L"";
			memset(prevValue, 0, sizeof(prevValue));
		}
	}

	Properties(aScene);
	AddComponentTab(aScene);

	ImGui::End();
	ImGui::End();
	ImGui::Begin("ViewPort");
	Vector2f windowWidth(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::Image((void*)GBuffer::GetVPSRV().Get(), { windowWidth.x, windowWidth.y });
	ImGuiViewport viewport;
	viewPortPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
	viewPortSize = windowWidth;
	if (someSelected)
	{
		entt::entity entity = selectedEntity;
		Transform transform = aScene->GetRegistry().get<TransformComponent>(entity).myTransform;
		EditorGuizmo(entity);
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
	static float timer = 0.0f;
	if (CANTDROPHERE)
	{
		timer += Timer::GetDeltaTime();
		if (timer >= 1)
		{
			timer = 0.0f;
			CANTDROPHERE = false;
		}
		ImGui::OpenPopup("CANT DROP HERE");
		if (ImGui::BeginPopup("CANT DROP HERE"))
		{
			ImGui::Text("CANT DROP HERE XD");
			ImGui::EndPopup();
		}
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
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	std::shared_ptr<SceneObject> object = scene->GetSceneObjects()[aIndex];
	std::filesystem::path objName = object->GetName();
	static std::vector<std::shared_ptr<SceneObject>> SelectedObjects;

	static bool acceptedDragDrop = false;

	if (!InputHandler::GetKeyIsHeld('M'))
	{
		if (SelectedObjects.size() > 0)
		{
			std::cout << "Cleared " << std::endl;
			SelectedObjects.clear();
		}
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
		{
			if (!acceptedDragDrop)
			{
				acceptedDragDrop = true;
				std::vector<std::shared_ptr<SceneObject>> child = *(std::vector<std::shared_ptr<SceneObject>>*)payload->Data;
				for (size_t i = 0; i < child.size(); i++)
				{
					if (child[i]->myParent)
						if (child[i]->myParent->myChildren.size() > 0)
							child[i]->myParent->myChildren.erase(std::remove(child[i]->myParent->myChildren.begin(),
								child[i]->myParent->myChildren.end(), child[i]));
					child[i]->myParent = nullptr;
				}
			}
		}
		else
		{
			acceptedDragDrop = false;
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::TreeNodeEx(&object->GetId(), ImGuiTreeNodeFlags_None, objName.string().c_str()))
	{
		if (InputHandler::GetKeyIsHeld('M'))
			if (ImGui::IsItemClicked())
			{
				std::cout << "Added Object: " << objName.string() << std::endl;
				SelectedObjects.push_back(object);
			}
		DragAndDrop(object, SelectedObjects, acceptedDragDrop);
		selectedEntity = object->myEntity;
		selectedItem = aIndex;
		ShowObjectChildren(object, SelectedObjects, acceptedDragDrop);
		ImGui::TreePop();
		someSelected = true;
	}
}

void EditorInterface::ShowObjectChildren(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop)
{
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();

	for (size_t j = 0; j < aObject->myChildren.size(); j++)
	{
		std::shared_ptr<SceneObject> objectsChild = aObject->myChildren[j];
		std::filesystem::path childName = objectsChild->GetName();
		if (ImGui::TreeNodeEx(&objectsChild->GetId(), ImGuiTreeNodeFlags_None,
			childName.string().c_str()))
		{
			if (InputHandler::GetKeyIsHeld('M'))
				if (ImGui::IsItemClicked())
				{
					std::cout << "Added Object: " << childName.string() << std::endl;
					aObjectVector.push_back(objectsChild);
				}
			DragAndDrop(objectsChild, aObjectVector, aAcceptedDragDrop);

			ShowObjectChildren(objectsChild, aObjectVector, aAcceptedDragDrop);
			selectedEntity = objectsChild->myEntity;
			selectedItem = static_cast<int>(j);
			someSelected = true;

			ImGui::TreePop();
		}
	}
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
			if (!aAcceptedDragDrop)
			{
				aAcceptedDragDrop = true;
				std::vector<std::shared_ptr<SceneObject>> child = *(std::vector<std::shared_ptr<SceneObject>>*)payload->Data;
				auto parent = aObject;
				for (size_t i = 0; i < child.size(); i++)
				{
					if (!HasConnection(child[i], parent))
					{
						if (child[i]->myParent)
							if (child[i]->myParent->myChildren.size() > 0)
							{
								child[i]->myParent->myChildren.erase(std::remove(child[i]->myParent->myChildren.begin(),
									child[i]->myParent->myChildren.end(), child[i]), child[i]->myParent->myChildren.end());
							}
						parent->myChildren.push_back(child[i]);
						child[i]->myParent = parent;
					}
				}
			}
		}
		else
		{
			aAcceptedDragDrop = false;
		}
		ImGui::EndDragDropTarget();
	}
}

bool EditorInterface::HasConnection(std::shared_ptr<SceneObject> aFirstObject, std::shared_ptr<SceneObject> aSecondObject)
{
	if (aFirstObject == aSecondObject)
		return true;
	std::queue<std::shared_ptr<SceneObject>> checkObjects;
	while (aFirstObject->myChildren.size() > 0)
	{
		for (size_t i = 0; i < aFirstObject->myChildren.size(); i++)
		{
			if (aFirstObject->myChildren[i] == aSecondObject)
				return true;
			checkObjects.push(aFirstObject->myChildren[i]);
		}
		if (!checkObjects.empty())
		{
			aFirstObject = checkObjects.front();
			checkObjects.pop();
		}
	}
	return false;
}

void EditorInterface::Properties(std::shared_ptr<Scene> aScene)
{
	ImGui::Begin("Properties");
	bool anythingOpen = false;
	if (someSelected)
	{
		if (aScene->GetRegistry().any_of<TransformComponent>(selectedEntity))
		{
			if (!anythingOpen && ImGui::CollapsingHeader("TransformComponent"))
			{
				Transform& transform = aScene->GetRegistry().get<TransformComponent>(selectedEntity).myTransform;
				static bool madeChange = false;
				static Transform oldTransform = transform;

				if (ImGui::DragFloat3("Position", &transform.GetPositionMutable().x, 1, -99999.f, 99999.f))
				{
					madeChange = true;
				}
				if (ImGui::DragFloat3("Rotation", &transform.GetRotationMutable().x, 1, -360, 360))
				{
					madeChange = true;
				}
				if (ImGui::DragFloat3("Scale", &transform.GetScaleMutable().x, 0.1f, -99999.f, 99999.f))
				{
					madeChange = true;
				}
				//if (!ImGui::IsAnyItemHovered())
				//{
				if (madeChange)
				{
					transform.ComposeMatrix();
					//Editor::EditorActions action;
					//action.MovedObject = true;
					//action.Object = scene->GetSceneObjects()[selectedItem];
					//action.OldTransform = oldTransform;
					//Editor::AddEditorAction(action);
					madeChange = false;
				}
				//}
				ImGui::NewLine();
				anythingOpen = true;
			}
		}

		if (aScene->GetRegistry().any_of<ModelComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("ModelComponent"))
			{
				std::shared_ptr<ModelInstance> mdl = aScene->GetRegistry().get<ModelComponent>(selectedEntity).myModel;

				static std::vector<std::filesystem::path> textureNames;
				static int selectedTexture = 0;
				static bool selectable = false;
				if (textureNames.size() > 0)
				{
					if (ImGui::BeginCombo("Models", textureNames[selectedTexture].string().c_str()))
					{
						for (size_t i = 0; i < textureNames.size(); i++)
						{
							if (ImGui::Selectable(textureNames[i].filename().string().c_str(), &selectable))
							{
								selectedTexture = static_cast<int>(i);
							}
							if (selectable)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (ImGui::Button("Load Texture"))
					{
						SetTexture(textureNames[selectedTexture]);
					}
					ImGui::NewLine();
				}

				if (mdl->HasBones() && mdl->GetAnimNames().size() >= 1)
				{
					static int selectedAnim = 0;
					static bool selectable1 = false;
					std::vector<std::filesystem::path> models;
					for (size_t i = 0; i < mdl->GetAnimNames().size(); i++)
					{
						models.push_back(mdl->GetAnimNames()[i]);
					}
					std::filesystem::path curAnimName(mdl->GetCurrentAnimation().myName);
					std::filesystem::path curAnim("Current Animation: " + curAnimName.filename().string());
					ImGui::Text(curAnim.string().c_str());

					if (ImGui::BeginCombo("Available Anims", models[selectedAnim].filename().string().c_str()))
					{
						for (size_t i = 0; i < models.size(); i++)
						{
							if (ImGui::Selectable(models[i].filename().string().c_str(), &selectable1))
							{
								selectedAnim = static_cast<int>(i);
							}
							if (selectable1)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (ImGui::Button("Set Animation"))
					{
						mdl->SetAnimation(mdl->GetAnimNames()[selectedAnim]);
					}
				}
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->RemoveModelInstance(aScene->GetRegistry().get<ModelComponent>(selectedEntity).myModel);
						aScene->GetRegistry().remove<ModelComponent>(selectedEntity);
					}
					ImGui::EndPopup();
				}

				if (ImGui::Button("Reload Textures"))
				{
					textureNames.clear();
					std::string path = ".\\Models\\Textures\\";
					for (const auto& file : directory_iterator(path))
					{
						textureNames.push_back(file.path());
					}
				}
				ImGui::NewLine();
			}
		}
		if (aScene->GetRegistry().any_of<TextComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("TextComponent"))
			{
				std::shared_ptr<Text> text = aScene->GetRegistry().get<TextComponent>(selectedEntity).myText;
				std::filesystem::path startName(text->GetText());
				char curValue[256];
				static char prevValue[256];
				memcpy(prevValue, startName.string().c_str(), sizeof(startName));
				memset(curValue, 0, sizeof(curValue));
				memcpy(curValue, prevValue, sizeof(prevValue));
				if (ImGui::InputText("Set Text", curValue, sizeof(curValue)))
				{
					std::filesystem::path name(curValue);
					text->SetText(name);
					memcpy(prevValue, curValue, sizeof(curValue));
				}
				bool is2D = text->GetIs2D();
				if (ImGui::Checkbox("2D", &is2D))
				{
					text->SetIs2D(is2D);
				}
				ImGui::NewLine();
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->RemoveText(aScene->GetRegistry().get<TextComponent>(selectedEntity).myText);
						aScene->GetRegistry().remove<TextComponent>(selectedEntity);
					}
					ImGui::EndPopup();
				}
			}
		}
		if (aScene->GetRegistry().any_of<ParticleSystemComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("ParticleSystemComponent"))
			{
				bool isDirty = false;
				std::shared_ptr<ParticleSystem> system = aScene->GetRegistry().get<ParticleSystemComponent>
					(selectedEntity).myParticleSystem;
				EmitterSettingsData& data = system->GetEmitters()[0].GetEmitterSettings();

				if (ImGui::DragFloat("Spawn Rate", &data.SpawnRate, 1, 0.1f, 999.f))
				{
					isDirty = true;
					if (data.SpawnRate <= 0)
					{
						data.SpawnRate = 0.1f;
					}
				}
				if (ImGui::DragFloat("LifeTime", &data.LifeTime, 1, 0.1f, 999.f))
				{
					isDirty = true;
					if (data.LifeTime <= 0)
					{
						data.LifeTime = 0.1f;
					}
				}

				float startVelocity[3] = { data.StartVelocity.x, data.StartVelocity.y, data.StartVelocity.z };
				ImGui::DragFloat3("Start Velocity", startVelocity, 1, -999.f, 999.f);
				data.StartVelocity = { startVelocity[0], startVelocity[1], startVelocity[2] };
				float endVelocity[3] = { data.EndVelocity.x, data.EndVelocity.y, data.EndVelocity.z };
				ImGui::DragFloat3("End Velocity", endVelocity, 1, -999.f, 999.f);
				data.EndVelocity = { endVelocity[0], endVelocity[1], endVelocity[2] };

				ImGui::DragFloat("Gravity Scale", &data.GravityScale, 1, 0, 999.f);
				ImGui::DragFloat("Start Size", &data.StartSize, 1, 0, 999.f);
				ImGui::DragFloat("End Size", &data.EndSize, 1, 0, 999.f);

				ImGui::ColorEdit4("Start Color", &data.StartColor.x);
				ImGui::ColorEdit4("End Color", &data.EndColor.x);

				ImGui::Checkbox("Looping", &data.Looping);
				ImGui::Checkbox("HasDuration", &data.HasDuration);
				if (data.HasDuration)
					ImGui::DragFloat("Duration", &data.Duration, 1, 0, 999.f);

				if (ImGui::Button("Refresh System"))
				{
					system->GetEmitters()[0].RefreshSystem();
				}

				if (isDirty)
					system->GetEmitters()[0].RefreshValues(data);
				ImGui::NewLine();
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->RemoveParticleSystem(aScene->GetRegistry().get<ParticleSystemComponent>(selectedEntity).myParticleSystem);
						aScene->GetRegistry().remove<ParticleSystemComponent>(selectedEntity);
					}
					ImGui::EndPopup();
				}
			}
		}
		if (aScene->GetRegistry().any_of<PlayerComponent>(selectedEntity))
		{
			if (ImGui::CollapsingHeader("PlayerComponent"))
			{
				ImGui::NewLine();
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->GetRegistry().remove<PlayerComponent>(selectedEntity);
					}
					ImGui::EndPopup();
				}
			}
		}
	}
}

void EditorInterface::AddComponentTab(std::shared_ptr<Scene> aScene)
{
	if (someSelected)
	{
		static bool addComponent = false;
		if (ImGui::Button("Add Component"))
		{
			addComponent = true;
		}
		if (addComponent)
		{
			static int selectedComponent = 0;
			std::vector<std::filesystem::path> components;
			static bool selectable = false;
			for (size_t i = 0; i < ComponentHandler::GetComponentNames().size(); i++)
			{
				components.push_back(ComponentHandler::GetComponentNames()[i]);
			}
			if (ImGui::BeginCombo("Components", components[selectedComponent].string().c_str()))
			{
				for (size_t i = 0; i < components.size(); i++)
				{
					if (ImGui::Selectable(components[i].string().c_str(), &selectable))
					{
						selectedComponent = static_cast<int>(i);
						ComponentHandler::AddComponent(selectedComponent, aScene->GetEntitys(ObjectType::All)[selectedItem]);
						addComponent = false;
					}
					if (selectable)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	ImGui::End();
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
	if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_back.dds"))
	{
		backArrow = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_back.dds");
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
				if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_directory.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_directory.dds");
				}
			}
			else if (currentItem.extension() == ".dds")
			{
				if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_texture.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_texture.dds");
				}
			}
			else if (currentItem.extension() == ".fbx")
			{
				if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_mesh.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_mesh.dds");
				}
			}
			else if (currentItem.extension() == ".cso")
			{
				if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_material.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_material.dds");
				}
			}
			else if (currentItem.extension() == ".json" || currentItem.extension() == ".scene")
			{
				if (TextureAssetHandler::LoadTexture(L".\\Editor\\Icons\\icon_file.dds"))
				{
					currentIcon = TextureAssetHandler::GetTexture(L".\\Editor\\Icons\\icon_file.dds");
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
						item.size() +1);
					ImGui::EndDragDropSource();
				}
			}
			ImGui::Text(currentItem.filename().string().c_str());
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

bool EditorInterface::EditorGuizmo(entt::entity aObjectMatrix)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	float view[16] = { 0 };
	float projection[16] = { 0 };
	float objectMatrix[16] = { 0 };
	Matrix4x4f viewMatrix = Matrix4x4f::GetFastInverse(scene->GetCamera()->GetTransform().GetMatrix());
	memcpy(&view[0], &viewMatrix, sizeof(float) * 16);
	memcpy(&projection[0], &scene->GetCamera()->GetProjection(), sizeof(float) * 16);
	memcpy(&objectMatrix[0], &scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetMatrix(), sizeof(float) * 16);
	static int action = 0;
	static bool change = false;
	if (InputHandler::GetKeyIsPressed('Q'))
		action = 0;
	if (InputHandler::GetKeyIsPressed('E'))
		action = 1;
	if (InputHandler::GetKeyIsPressed('R'))
		action = 2;
	Transform test;
	if (action == 0)
		if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &objectMatrix[0]))
		{
			change = true;
			//SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetPosition({ objectMatrix[12],objectMatrix[13], objectMatrix[14] });
			//ImGui::PopStyleVar();
			//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
			//return true;
		}
	if (action == 1)
		if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::ROTATE, ImGuizmo::WORLD, &objectMatrix[0]))
		{
			change = true;
			//SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetRotation({ objectMatrix[12],objectMatrix[13], objectMatrix[14] });
			//ImGui::PopStyleVar();
			//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
			//return true;
		}
	if (action == 2)
		if (ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::SCALE, ImGuizmo::WORLD, &objectMatrix[0]))
		{
			change = true;
			//ImGui::PopStyleVar();
			//std::cout << "  After " + std::to_string(SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.GetPosition().x) << std::endl;
			//return true;
		}
	if (change)
	{
		memcpy(&test.GetMatrix(), &objectMatrix[0], sizeof(float) * 16);
		Vector3f pos, rot, scale;
		test.GetMatrix().Deconstruct(pos, rot, scale);
		switch (action)
		{
		case 0:
			scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetPosition(pos);
			break;
		case 1:
			scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetRotation(rot);
			break;
		case 2:
			scene->GetRegistry().get<TransformComponent>(aObjectMatrix).myTransform.SetScale(scale);
			break;
		}
		ImGui::PopStyleVar();
		change = false;
		return true;
	}
	ImGui::PopStyleVar();

	return false;
}