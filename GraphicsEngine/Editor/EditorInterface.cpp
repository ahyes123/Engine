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

void EditorInterface::SceneHierchy()
{
#ifdef _DEBUG

#endif

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
				//Do something
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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Performance"))
		{
			ImGui::Begin("Performance");
			//ImGui::Text("Draw calls: %d.", Renderer::GetDrawCallCount());
			//ImGui::Text("Point lights: %d.", Renderer::GetPointLightCount());
			//ImGui::Text("Directional lights: %d.", Renderer::GetDirlightCount());
			ImGui::Text("Delta Time %.3f", ImGui::GetIO().DeltaTime);
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::End();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Game Objects"))
		{
			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					SceneHandler::GetActiveScene()->AddModelInstance(ModelAssetHandler::CreateCube(L"Cube"));
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
			static bool colorChange = false;
			if (InputHandler::GetKeyIsPressed(VK_ESCAPE))
			{
				openedSettings = false;
				colorChange = false;
			}
			ImGui::Begin("Settings");
			ImGui::InputFloat("Camera Speed", &SceneHandler::GetActiveScene()->GetCamera()->GetCameraSpeed());

			float timeScale = CommonUtilities::Timer::GetTimeScale();
			ImGui::InputFloat("Time Scale", &timeScale);
			CommonUtilities::Timer::SetTimeScale(timeScale);
			ImGui::ColorEdit4("Clear Color", &GraphicsEngine::GetClearColor()[0]);
			ImGui::End();
		}
		ImGui::EndMainMenuBar();
	}


	Editor::EditorActionHandler();
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	ImGui::Begin("Scene Hierchy");

	static bool canChangeName = false;
	bool someThingSelected = false;
	for (size_t i = 0; i < scene->GetSceneObjects().size(); i++)
	{
		static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
		std::filesystem::path name(scene->GetSceneObjects()[i]->GetName());
		if (ImGui::TreeNodeEx((void*)scene->GetSceneObjects()[i]->GetId(), flags, name.string().c_str()))
		{
			ImGui::TreePop();
			selectedItem = i;
			someThingSelected = true;
		}

		if (someThingSelected)
		{
			static bool deleteItem = false;
			if (ImGui::BeginPopupContextWindow(std::to_string(scene->GetSceneObjects()[i]->GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Change Name"))
				{
					canChangeName = true;
				}
				if (scene->GetRegistry().any_of<ModelComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]))
				{
					if (scene->GetRegistry().get<ModelComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]).myModel->HasBones())
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
				entt::entity ent = scene->GetEntitys(ObjectType::All)[selectedItem];
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				action.RemovedObject = true;
				action.Object = scene->GetSceneObjects()[selectedItem];
				action.oldEntity = ent;
				Editor::AddEditorAction(action);

				if (reg.any_of<ModelComponent>(ent))
					scene->RemoveModelInstance(reg.get<ModelComponent>(ent).myModel);
				if (reg.any_of<ParticleSystemComponent>(ent))
					scene->RemoveParticleSystem(reg.get<ParticleSystemComponent>(ent).myParticleSystem);
				if (reg.any_of<TextComponent>(ent))
					scene->RemoveText(reg.get<TextComponent>(ent).myText);
				someThingSelected = false;
			}
		}
		if (i >= scene->GetSceneObjects().size() - 1 && someThingSelected == false)
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
			oldName = scene->GetSceneObjects()[selectedItem]->GetName();

		if (ImGui::InputText("Name", curValue, sizeof(curValue)))
		{
			memcpy(prevValue, curValue, sizeof(curValue));
			std::filesystem::path name(curValue);
			scene->GetSceneObjects()[selectedItem]->SetName(name);
		}
		if (InputHandler::GetKeyIsPressed(VK_RETURN))
		{
			Editor::EditorActions action;
			action.ChangedName = true;
			action.Object = scene->GetSceneObjects()[selectedItem];
			action.OldName = oldName;
			Editor::AddEditorAction(action);
			canChangeName = false;
			oldName = L"";
			memset(prevValue, 0, sizeof(prevValue));
		}
	}
	ImGui::Begin("Properties");
	if (someThingSelected)
	{
		if (scene->GetRegistry().any_of<TransformComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			Transform& transform = scene->GetRegistry().get<TransformComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]).myTransform;
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
		}
		if (scene->GetRegistry().any_of<ModelComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			std::shared_ptr<ModelInstance> mdl = scene->GetRegistry().get<ModelComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]).myModel;

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
		}
		if (scene->GetRegistry().any_of<TextComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			std::shared_ptr<Text> text = scene->GetRegistry().get<TextComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]).myText;
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
		}
		if (scene->GetRegistry().any_of<ParticleSystemComponent>(scene->GetEntitys(ObjectType::All)[selectedItem]))
		{
			bool isDirty = false;
			std::shared_ptr<ParticleSystem> system = scene->GetRegistry().get<ParticleSystemComponent>
				(scene->GetEntitys(ObjectType::All)[selectedItem]).myParticleSystem;
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
		}

		entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
		entt::entity ent = scene->GetEntitys(ObjectType::All)[selectedItem];

		if (reg.any_of<ModelComponent>(ent))
		{
			ImGui::Text("ModelComponent");
		}
		if (reg.any_of<PlayerComponent>(ent))
		{
			ImGui::Text("PlayerComponent");
		}
		if (reg.any_of<TransformComponent>(ent))
		{
			ImGui::Text("TransformComponent");
		}
		if (reg.any_of<ParticleSystemComponent>(ent))
		{
			ImGui::Text("ParticleComponent");
		}
		if (reg.any_of<TextComponent>(ent))
		{
			ImGui::Text("TextComponent");
		}

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
						ComponentHandler::AddComponent(selectedComponent, scene->GetEntitys(ObjectType::All)[selectedItem]);
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
	ImGui::End();
	bool f;
	ImGui::Begin("ViewPort",&f, ImGuiWindowFlags_NoInputs);
	Vector2f windowWidth(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
	ImGui::Image((void*)GBuffer::GetVPSRV().Get(), { windowWidth.x, windowWidth.y });
	ImGuiViewport viewport;
	if(someThingSelected)
	{
		entt::entity entity = scene->GetEntitys(ObjectType::All)[selectedItem];
		Transform& transform = scene->GetRegistry().get<TransformComponent>(entity).myTransform;
		EditorGuizmo(transform.GetMatrix());
	}
	ImGui::End();
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
						SceneHandler::GetActiveScene()->AddModelInstance(model);
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
						SceneHandler::GetActiveScene()->AddModelInstance(model);
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

bool EditorInterface::EditorGuizmo(Matrix4x4f& aObjectMatrix)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	//RECT windowRect = GraphicsEngine::GetWindowRectangle();
	//ImGui::SetWindowPos(ImVec2{ static_cast<float>(windowRect.left), static_cast<float>(windowRect.top) });
	//ImGui::SetWindowSize(ImVec2{ static_cast<float>(windowRect.right - windowRect.left), static_cast<float>(windowRect.bottom - windowRect.top) });
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	float view[16] = { 0 };
	float projection[16] = { 0 };
	float objectMatrix[16] = { 0 };
	memcpy(view, &SceneHandler::GetActiveScene()->GetCamera()->GetTransform().GetMatrix(), sizeof(float) * 16);
	memcpy(projection, &SceneHandler::GetActiveScene()->GetCamera()->GetProjection(), sizeof(float) * 16);
	memcpy(objectMatrix, &aObjectMatrix, sizeof(float) * 16);
	ImGuizmo::Manipulate(&view[0], &projection[0], ImGuizmo::TRANSLATE, ImGuizmo::WORLD, &objectMatrix[0]);
	ImGui::PopStyleVar();
	return true;
}