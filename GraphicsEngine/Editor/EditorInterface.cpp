#include "GraphicsEngine.pch.h"
#include "EditorInterface.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"
#include <filesystem>
#include "../GraphicsEngine.h"
#include "../Tools/InputHandler.h"
#include "../Tools/Timer.h"
#include "Editor.h"
#include "../Scene/SceneHandler.h"
#include <commdlg.h>
#include <fstream>

#include "DX11.h"
#include "../Engine/ComponentHandler.h"
#include "../Particle/ParticleAssetHandler.h"
#include "../Text/TextFactory.h"

using std::filesystem::directory_iterator;
using namespace CommonUtilities;

bool EditorInterface::addAnimation;
int EditorInterface::selectedItem;

void EditorInterface::ShowEditor()
{
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	bool someThingSelected = false;
	Editor::EditorActionHandler();
	EnableDocking();
	MenuBar();
	ModelLoader();
	SceneHierchy(someThingSelected, scene);
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
	if (ImGui::BeginMainMenuBar())
	{
		static bool createScene = false;
		static bool openedSettings = false;
		static bool createParticleSystem = false;
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
					std::ofstream fileName("Json/Scenes/" + name.string() + ".json");

					using nlohmann::json;
					json j;
					j["SceneName"] = name.string();
					j["Size"] = 0;
					std::ofstream oStream("Json/Scenes/" + name.string() + ".json");
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
				ofn.lpstrFilter = "Scene (.json)\0*.json\0";
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
					SceneHandler::GetActiveScene()->AddModelInstance(ModelAssetHandler::CreateCube(L"Cube"), entity);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Particle System"))
			{
				entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
				EmitterSettingsData data;
				std::shared_ptr<ParticleSystem> system = ParticleAssetHandler::CreateParticleSystem(data);
				SceneHandler::GetActiveScene()->AddParticleSystem(system, entity);
			}
			if (ImGui::MenuItem("Text Element"))
			{
				entt::entity entity = SceneHandler::GetActiveScene()->GetRegistry().create();
				std::shared_ptr<Text> text = TextFactory::CreateText(L"Text", 1, 12, true);
				SceneHandler::GetActiveScene()->AddText(text, entity);
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

			ImGui::DragFloat("Camera Speed", &SceneHandler::GetActiveScene()->GetCamera()->GetCameraSpeed(), 0.1f, 0, INT_MAX);

			float timeScale = CommonUtilities::Timer::GetTimeScale();
			ImGui::DragFloat("Time Scale", &timeScale, 0.05f, 0, INT_MAX);
			CommonUtilities::Timer::SetTimeScale(timeScale);
			ImGui::ColorEdit4("Clear Color", &GraphicsEngine::GetClearColor()[0]);
			ImGui::Checkbox("Auto Save", &GraphicsEngine::GetAutoSave());
			ImGui::NewLine();

			if (ImGui::Button("Save Settings"))
			{
				Editor::SaveSettings();
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
		for (int i = models.size() - 1; i >= 0; i--)
		{
			models.erase(models.begin() + i);
		}
		for (int i = animations.size() - 1; i >= 0; i--)
		{
			animations.erase(animations.begin() + i);
		}
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
					selectedModel = i;
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
			std::shared_ptr<ModelInstance> mdl = SceneHandler::GetActiveScene()->GetModels()[selectedItem];
			if (!mdl->HasBones())
			{
				addAnimation = false;
			}
			if (ImGui::Button("Add Animation"))
			{
				std::filesystem::path name(animations[selectedAnimation]);
				ModelAssetHandler::LoadAnimation(mdl->GetPath(), name);
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
						Editor::AddEditorAction(action);
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
							selectedAnimation = i;
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
						Editor::AddEditorAction(action);
					}
				}
			}
		}
	}
	ImGui::End();
}

void EditorInterface::SceneHierchy(bool aSomeThingSelected, std::shared_ptr<Scene> aScene)
{
	//EnableDocking();
	//MenuBar();
	ImGui::Begin("Scene Hierchy");

	static bool canChangeName = false;
	for (size_t i = 0; i < aScene->GetSceneObjects().size(); i++)
	{
		static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
		std::filesystem::path name(aScene->GetSceneObjects()[i]->GetName());
		if (ImGui::TreeNodeEx((void*)aScene->GetSceneObjects()[i]->GetId(), flags, name.string().c_str()))
		{
			ImGui::TreePop();
			selectedItem = i;
			aSomeThingSelected = true;
		}

		if (aSomeThingSelected)
		{
			static bool deleteItem = false;
			if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[i]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Change Name"))
				{
					canChangeName = true;
				}
				if (aScene->GetRegistry().any_of<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
				{
					if (aScene->GetRegistry().get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel->HasBones())
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
				Editor::EditorActions action;
				entt::entity ent = aScene->GetEntitys(ObjectType::All)[selectedItem];
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				action.RemovedObject = true;
				action.Object = aScene->GetSceneObjects()[selectedItem];
				action.oldEntity = ent;
				Editor::AddEditorAction(action);

				if (reg.any_of<ModelComponent>(ent))
					aScene->RemoveModelInstance(reg.get<ModelComponent>(ent).myModel);
				if (reg.any_of<ParticleSystemComponent>(ent))
					aScene->RemoveParticleSystem(reg.get<ParticleSystemComponent>(ent).myParticleSystem);
				if (reg.any_of<TextComponent>(ent))
					aScene->RemoveText(reg.get<TextComponent>(ent).myText);
				aSomeThingSelected = false;
			}
			static bool stillPressed = false;
			if (!stillPressed)
			{
				if (InputHandler::GetKeyIsPressed('V'))
				{
					Editor::EditorActions action;
					entt::entity entity = ComponentHandler::DuplicateEntity(aScene->GetEntitys(ObjectType::All)[selectedItem]);
					action.AddedObject = true;
					action.oldEntity = entity;
					entt::registry& reg = aScene->GetRegistry();

					if (reg.all_of<ModelComponent>(entity))
					{
						std::shared_ptr<ModelInstance> mdl;
						if (!reg.get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel->HasBones())
						{
							mdl = ModelAssetHandler::LoadModel(reg.get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel->GetPath());
						}
						else
						{
							mdl = ModelAssetHandler::LoadModelWithAnimation(reg.get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel->GetPath(),
								reg.get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel->GetCurrentAnimation().myName);
						}
						reg.get<ModelComponent>(entity).myModel = mdl;
						aScene->AddModelInstance(reg.get<ModelComponent>(entity).myModel, entity);
						action.Object = reg.get<ModelComponent>(entity).myModel;
					}
					if (reg.all_of<TextComponent>(entity))
					{
						std::shared_ptr<Text> text = reg.get<TextComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myText;
						aScene->AddText(TextFactory::CreateText(text->GetText(), 1, text->GetFont()->Atlas.Size, text->GetIs2D()), entity);
						reg.get<TextComponent>(entity).myText = reg.get<TextComponent>(entity).myText;
						action.Object = reg.get<TextComponent>(entity).myText;
					}
					if (reg.all_of<ParticleSystemComponent>(entity))
					{
						std::shared_ptr<ParticleSystem> system = reg.get<ParticleSystemComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myParticleSystem;
						aScene->AddParticleSystem(ParticleAssetHandler::CreateParticleSystem(system->GetEmitters()[0].GetEmitterSettings()), entity);
						reg.get<ParticleSystemComponent>(entity).myParticleSystem = reg.get<ParticleSystemComponent>(entity).myParticleSystem;
						action.Object = reg.get<ParticleSystemComponent>(entity).myParticleSystem;
					}
					Editor::AddEditorAction(action);
					stillPressed = true;
				}
			}
			if (InputHandler::GetKeyWasReleased('V'))
			{
				stillPressed = false;
			}
		}
		if (i >= aScene->GetSceneObjects().size() - 1 && aSomeThingSelected == false)
		{
			selectedItem = -1;
			canChangeName = false;
		}
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
			aScene->GetSceneObjects()[selectedItem]->SetName(name);
		}
		if (InputHandler::GetKeyIsPressed(VK_RETURN))
		{
			Editor::EditorActions action;
			action.ChangedName = true;
			action.Object = aScene->GetSceneObjects()[selectedItem];
			action.OldName = oldName;
			Editor::AddEditorAction(action);
			canChangeName = false;
			oldName = L"";
			memset(prevValue, 0, sizeof(prevValue));
		}
	}

	Properties(aSomeThingSelected, aScene);
	AddComponentTab(aSomeThingSelected, aScene);

	ImGui::End();
	ImGui::Begin("ViewPort");
	Vector2f windowWidth(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::Image((void*)GBuffer::GetVPSRV().Get(), { windowWidth.x, windowWidth.y });
	ImGuiViewport viewport;
	if (aSomeThingSelected)
	{
		entt::entity entity = aScene->GetEntitys(ObjectType::All)[selectedItem];
		Transform transform = aScene->GetRegistry().get<TransformComponent>(entity).myTransform;
		EditorGuizmo(entity);
		//scene->GetRegistry().get<TransformComponent>(entity).myTransform.SetPosition(transform.GetPosition());
	}
	ImGui::End();
	ImGui::End();
}

void EditorInterface::Properties(bool aSomeThingSelected, std::shared_ptr<Scene> aScene)
{
	ImGui::Begin("Properties");
	bool anythingOpen = false;
	if (aSomeThingSelected)
	{
		if (aScene->GetRegistry().any_of<TransformComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			if (!anythingOpen && ImGui::CollapsingHeader("TransformComponent"))
			{
				Transform& transform = aScene->GetRegistry().get<TransformComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myTransform;
				static bool madeChange = false;
				static Transform oldTransform = transform;

				if (ImGui::DragFloat3("Position", &transform.GetPositionMutable().x, 1, -INT_MAX, INT_MAX))
				{
					madeChange = true;
				}
				if (ImGui::DragFloat3("Rotation", &transform.GetRotationMutable().x, 1, -360, 360))
				{
					madeChange = true;
				}
				if (ImGui::DragFloat3("Scale", &transform.GetScaleMutable().x, 0.1f, -INT_MAX, INT_MAX))
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

		if (aScene->GetRegistry().any_of<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			if (ImGui::CollapsingHeader("ModelComponent"))
			{
				std::shared_ptr<ModelInstance> mdl = aScene->GetRegistry().get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel;

				if (mdl->HasBones() && mdl->GetAnimNames().size() >= 1)
				{
					static int selectedAnim = 0;
					static bool selectable = false;
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
							if (ImGui::Selectable(models[i].filename().string().c_str(), &selectable))
							{
								selectedAnim = i;
							}
							if (selectable)
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
				ImGui::NewLine();
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->RemoveModelInstance(aScene->GetRegistry().get<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myModel);
						aScene->GetRegistry().remove<ModelComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]);
					}
					ImGui::EndPopup();
				}
			}
		}
		if (aScene->GetRegistry().any_of<TextComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			if (ImGui::CollapsingHeader("TextComponent"))
			{
				std::shared_ptr<Text> text = aScene->GetRegistry().get<TextComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myText;
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
						aScene->RemoveText(aScene->GetRegistry().get<TextComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myText);
						aScene->GetRegistry().remove<TextComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]);
					}
					ImGui::EndPopup();
				}
			}
		}
		if (aScene->GetRegistry().any_of<ParticleSystemComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			if (ImGui::CollapsingHeader("ParticleSystemComponent"))
			{
				bool isDirty = false;
				std::shared_ptr<ParticleSystem> system = aScene->GetRegistry().get<ParticleSystemComponent>
					(aScene->GetEntitys(ObjectType::All)[selectedItem]).myParticleSystem;
				EmitterSettingsData& data = system->GetEmitters()[0].GetEmitterSettings();

				if (ImGui::DragFloat("Spawn Rate", &data.SpawnRate, 1, 0.1f, INT_MAX))
				{
					isDirty = true;
					if (data.SpawnRate <= 0)
					{
						data.SpawnRate = 0.1f;
					}
				}
				if (ImGui::DragFloat("LifeTime", &data.LifeTime, 1, 0.1f, INT_MAX))
				{
					isDirty = true;
					if (data.LifeTime <= 0)
					{
						data.LifeTime = 0.1f;
					}
				}

				float startVelocity[3] = { data.StartVelocity.x, data.StartVelocity.y, data.StartVelocity.z };
				ImGui::DragFloat3("Start Velocity", startVelocity, 1, -INT_MAX, INT_MAX);
				data.StartVelocity = { startVelocity[0], startVelocity[1], startVelocity[2] };
				float endVelocity[3] = { data.EndVelocity.x, data.EndVelocity.y, data.EndVelocity.z };
				ImGui::DragFloat3("End Velocity", endVelocity, 1, -INT_MAX, INT_MAX);
				data.EndVelocity = { endVelocity[0], endVelocity[1], endVelocity[2] };

				ImGui::DragFloat("Gravity Scale", &data.GravityScale, 1, 0, INT_MAX);
				ImGui::DragFloat("Start Size", &data.StartSize, 1, 0, INT_MAX);
				ImGui::DragFloat("End Size", &data.EndSize, 1, 0, INT_MAX);

				ImGui::ColorEdit4("Start Color", &data.StartColor.x);
				ImGui::ColorEdit4("End Color", &data.EndColor.x);

				ImGui::Checkbox("Looping", &data.Looping);
				ImGui::Checkbox("HasDuration", &data.HasDuration);
				if (data.HasDuration)
					ImGui::DragFloat("Duration", &data.Duration, 1, 0, INT_MAX);

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
						aScene->RemoveParticleSystem(aScene->GetRegistry().get<ParticleSystemComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]).myParticleSystem);
						aScene->GetRegistry().remove<ParticleSystemComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]);
					}
					ImGui::EndPopup();
				}
			}
		}
		if (aScene->GetRegistry().any_of<PlayerComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			if (ImGui::CollapsingHeader("PlayerComponent"))
			{
				ImGui::NewLine();
				if (ImGui::BeginPopupContextWindow(std::to_string(aScene->GetSceneObjects()[selectedItem]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Remove Component"))
					{
						aScene->GetRegistry().remove<PlayerComponent>(aScene->GetEntitys(ObjectType::All)[selectedItem]);
					}
					ImGui::EndPopup();
				}
			}
		}
	}
}

void EditorInterface::AddComponentTab(bool aSomeThingSelected, std::shared_ptr<Scene> aScene)
{
	if (aSomeThingSelected)
	{
		entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
		entt::entity ent = aScene->GetEntitys(ObjectType::All)[selectedItem];

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
						selectedComponent = i;
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