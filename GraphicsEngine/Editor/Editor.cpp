#include "GraphicsEngine.pch.h"
#include "Editor.h"
#include "../Model/ModelInstance.h"
#include "../Tools/InputHandler.h"
#include "../GraphicsEngine.h"
#include <filesystem>
#include <fstream>
#include "../Scene/SceneHandler.h"
#include "../Tools/Timer.h"
#include "../Text/TextFactory.h"
#include "../Particle/ParticleAssetHandler.h"

#include "../Engine/ComponentHandler.h"
#include "../Engine/Player.h"

using std::filesystem::directory_iterator;

std::vector<Editor::EditorActions> Editor::myUndoActions;
std::vector<Editor::EditorActions> Editor::myRedoActions;

void Editor::AddUndoAction(const EditorActions& anAction, const bool& aNewAction)
{
	myUndoActions.push_back(anAction);
	if (myUndoActions.size() >= 15)
		myUndoActions.erase(myUndoActions.begin() + 0);
	if (aNewAction)
		myRedoActions.clear();
}

void Editor::AddRedoActoin(const EditorActions& anAction)
{
	myRedoActions.push_back(anAction);
	if (myRedoActions.size() >= 15)
		myRedoActions.erase(myRedoActions.begin() + 0);
}

void Editor::EditorActionHandler()
{
	if (CommonUtilities::InputHandler::GetKeyIsHeld(VK_CONTROL) && CommonUtilities::InputHandler::GetKeyIsPressed('Z'))
	{
		if (myUndoActions.size() > 0)
		{
			EditorActions action = myUndoActions[myUndoActions.size() - 1];
			if (action.AddedObject)
			{
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				if (reg.any_of<ModelComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveModelInstance(reg.get<ModelComponent>(action.oldEntity).myModel);
				if (reg.any_of<ParticleSystemComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveParticleSystem(reg.get<ParticleSystemComponent>(action.oldEntity).myParticleSystem);
				if (reg.any_of<TextComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveText(reg.get<TextComponent>(action.oldEntity).myText);
			}
			else if (action.ChangedName)
			{
				action.beforeName = action.Object->GetName();
				action.Object->SetName(action.OldName);
			}
			else if (action.MovedObject)
			{
				action.beforeTransform = action.Object->GetTransform();
				action.Object->SetTransform(action.OldTransform);
			}
			else if (action.RemovedObject)
			{
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				if (reg.any_of<ModelComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddModelInstance(reg.get<ModelComponent>(action.oldEntity).myModel, action.oldEntity);
				if (reg.any_of<ParticleSystemComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddParticleSystem(reg.get<ParticleSystemComponent>(action.oldEntity).myParticleSystem, action.oldEntity);
				if (reg.any_of<TextComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddText(reg.get<TextComponent>(action.oldEntity).myText, action.oldEntity);
			}
			AddRedoActoin(action);
			myUndoActions.erase(myUndoActions.begin() + myUndoActions.size() - 1);
		}
	}
	if ((CommonUtilities::InputHandler::GetKeyIsHeld(VK_CONTROL) && CommonUtilities::InputHandler::GetKeyIsPressed('Y')) ||
		(CommonUtilities::InputHandler::GetKeyIsHeld(VK_CONTROL) && CommonUtilities::InputHandler::GetKeyIsHeld(VK_SHIFT)
			&& CommonUtilities::InputHandler::GetKeyIsPressed('Z')))
	{
		if (myRedoActions.size() > 0)
		{
			EditorActions action = myRedoActions[myRedoActions.size() - 1];
			if (action.AddedObject)
			{
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				if (reg.any_of<ModelComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddModelInstance(reg.get<ModelComponent>(action.oldEntity).myModel, action.oldEntity);
				if (reg.any_of<ParticleSystemComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddParticleSystem(reg.get<ParticleSystemComponent>(action.oldEntity).myParticleSystem,
						action.oldEntity);
				if (reg.any_of<TextComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddText(reg.get<TextComponent>(action.oldEntity).myText, action.oldEntity);
			}
			else if (action.ChangedName)
			{
				action.Object->SetName(action.beforeName);
			}
			else if (action.MovedObject)
			{
				action.Object->SetTransform(action.beforeTransform);
			}
			else if (action.RemovedObject)
			{
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				if (reg.any_of<ModelComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveModelInstance(reg.get<ModelComponent>(action.oldEntity).myModel);
				if (reg.any_of<ParticleSystemComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveParticleSystem(reg.get<ParticleSystemComponent>(action.oldEntity).myParticleSystem);
				if (reg.any_of<TextComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->RemoveText(reg.get<TextComponent>(action.oldEntity).myText);
			}
			AddUndoAction(action, false);
			myRedoActions.erase(myRedoActions.begin() + myRedoActions.size() - 1);
		}
	}
}

void Editor::SaveCurrentScene()
{
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	nlohmann::json json;
	const std::string path = "./Json/Scenes/";
	bool isFileFound = false;
	std::filesystem::path sceneName(scene->GetSceneName() + L".scene");
	for (const auto& file : directory_iterator(path))
	{
		if (sceneName == file.path().filename().string())
		{
			isFileFound = true;
			break;
		}
	}
	if (!isFileFound)
	{
		//std::ofstream file(sceneName);
		std::cout << "File Not Found" << std::endl;
	}
	else
	{
		std::cout << "File Found" << std::endl;
	}
	json["SceneName"] = sceneName.string();
	json["Size"] = scene->GetSceneObjects().size();

	int num = 0;
	SaveModels(scene, json, num);
	SaveTexts(scene, json, num);
	SaveParticleSystems(scene, json, num);

	std::ofstream oStream(path + sceneName.string());
	oStream << json;
}

void Editor::SaveScenes()
{
	for (size_t i = 0; i < SceneHandler::GetScenes().size(); i++)
	{
		std::shared_ptr<Scene> scene = SceneHandler::GetScenes()[i];
		nlohmann::json json;
		const std::string path = "./Json/Scenes/";
		bool isFileFound = false;
		std::filesystem::path sceneName(SceneHandler::GetScenes()[i]->GetSceneName() + L".scene");
		for (const auto& file : directory_iterator(path))
		{
			if (sceneName == file.path().filename().string())
			{
				isFileFound = true;
				break;
			}
		}
		if (!isFileFound)
		{
			//std::ofstream file(sceneName);
			std::cout << "File Not Found" << std::endl;
		}
		else
		{
			std::cout << "File Found" << std::endl;
		}
		json["SceneName"] = sceneName.string();
		json["Size"] = scene->GetAllSceneObjects().size();

		int num = 0;
		SaveModels(scene, json, num);
		SaveTexts(scene, json, num);
		SaveParticleSystems(scene, json, num);
		SaveParents(scene, json, num);

		std::ofstream oStream(path + sceneName.string());
		oStream << json;
	}
}

void Editor::LoadCurrentScene()
{
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	nlohmann::json json;
	std::ifstream ifStream(L"Json/Scenes/" + scene->GetSceneName() + L".scene");
	if (ifStream.fail())
		return;
	ifStream >> json;
	int index = json["Size"];
	for (size_t i = 0; i < json["Size"]; i++)
	{
		std::string num = std::to_string(i);
		for (size_t j = 0; j < json[num]["Components"].size(); j++)
		{
			switch (static_cast<int>(json[num]["Components"][std::to_string(j)]))
			{
			case 0: //ModelComponent
				LoadModels(scene, json, num);
				break;
			case 3: //ParticleSystem
				LoadParticleSystems(scene, json, num);
				break;
			case 4: //TextComponent
				LoadTexts(scene, json, num);
				break;
			}
		}
	}
	LoadParents(scene, json, index);
}

void Editor::SaveComponents(nlohmann::json& aJson, std::string& aNum, entt::entity aEntity, std::shared_ptr<Scene> aScene)
{
	std::vector<int> ids;
	entt::registry& reg = aScene->GetRegistry();

	if (reg.any_of<ModelComponent>(aEntity))
	{
		std::filesystem::path name(reg.get<ModelComponent>(aEntity).myModel->GetName());
		aJson[aNum]["Name"] = name;
		ids.push_back(0);
	}
	if (reg.any_of<PlayerComponent>(aEntity))
	{
		ids.push_back(1);
	}
	if (reg.any_of<TransformComponent>(aEntity))
	{
		Transform& transform = reg.get<TransformComponent>(aEntity).myTransform;
		aJson[aNum]["Position"] = { transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z };
		aJson[aNum]["Scale"] = { transform.GetScale().x, transform.GetScale().y, transform.GetScale().z };
		aJson[aNum]["Rotation"] = { transform.GetRotation().x,transform.GetRotation().y, transform.GetRotation().z };
		ids.push_back(2);
	}
	if (reg.any_of<ParticleSystemComponent>(aEntity))
	{
		std::filesystem::path name(reg.get<ParticleSystemComponent>(aEntity).myParticleSystem->GetName());
		aJson[aNum]["Name"] = name;
		ids.push_back(3);
	}
	if (reg.any_of<TextComponent>(aEntity))
	{
		std::filesystem::path name(reg.get<TextComponent>(aEntity).myText->GetName());
		aJson[aNum]["Name"] = name;
		ids.push_back(4);
	}
	for (size_t k = 0; k < ids.size(); k++)
	{
		int id = ids[k];
		aJson[aNum]["Components"][std::to_string(k)] = id;
	}
}

void Editor::LoadComponents(nlohmann::json& aJson, std::string& aNum, entt::entity aEntity, std::shared_ptr<Scene> aScene)
{
	for (size_t k = 0; k < aJson[aNum]["Components"].size(); k++)
	{
		int compId = aJson[aNum]["Components"][std::to_string(k)];
		ComponentHandler::AddComponent(compId, aEntity);
	}
	entt::registry& reg = aScene->GetRegistry();
	if (reg.any_of<TransformComponent>(aEntity))
	{
		Vector3f position = { aJson[aNum]["Position"][0], aJson[aNum]["Position"][1], aJson[aNum]["Position"][2] };
		Vector3f scale = { aJson[aNum]["Scale"][0], aJson[aNum]["Scale"][1], aJson[aNum]["Scale"][2] };
		Vector3f rotation = { aJson[aNum]["Rotation"][0], aJson[aNum]["Rotation"][1], aJson[aNum]["Rotation"][2] };
		reg.get<TransformComponent>(aEntity).myTransform.SetPosition(position);
		reg.get<TransformComponent>(aEntity).myTransform.SetScale(scale);
		reg.get<TransformComponent>(aEntity).myTransform.SetRotation(rotation);
	}
}

void Editor::SaveParents(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum)
{
	auto obj = SceneHandler::GetActiveScene()->GetAllSceneObjects();
	
	for (size_t i = 0; i < aNum; i++)
	{
		std::string num = std::to_string(i);
		bool children = obj[i]->myChildren.size();
		for (size_t j = 0; j < obj[i]->myChildren.size(); j++)
		{
			aJson[num]["Children"][j] = obj[i]->myChildren[j]->GetId();
		}
		if (!children)
			aJson[num]["Children"] = {};

		aJson[num]["ID"] = obj[i]->GetId();
	}
}

void Editor::LoadParents(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum)
{
	auto obj = SceneHandler::GetActiveScene()->GetAllSceneObjects();
	for (size_t i = 0; i < aNum; i++)
	{
		std::string num = std::to_string(i);
		obj[i]->SetId(aJson[num]["ID"]);
	}
	for (size_t i = 0; i < aNum; i++)
	{
		std::string num = std::to_string(i);
		for (size_t j = 0; j < aJson[num]["Children"].size(); j++)
		{
			auto child = SceneHandler::GetActiveScene()->GetObjectByID(aJson[num]["Children"][j]);
			if (!child)
				continue;
			obj[i]->myChildren.push_back(child);
			child->myParent = obj[i];
		}
	}
}

void Editor::SaveModels(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum)
{
	for (size_t i = 0; i < aScene->GetModels().size(); i++)
	{
		std::string num = std::to_string(aNum);
		std::shared_ptr<ModelInstance> mdl = aScene->GetModels()[i];
		std::filesystem::path path(mdl->GetPath());
		aJson[num]["ModelPath"] = path;
		if (mdl->GetCurrentAnimation().myFrames.size() > 0)
		{
			aJson[num]["IsAnim"] = true;
			std::filesystem::path animName(mdl->GetCurrentAnimation().myName);
			aJson[num]["AnimationPaths"]["0"] = animName;
			int id = 1;
			for (size_t k = 0; k < mdl->GetAnimNames().size(); k++)
			{
				if (animName != mdl->GetAnimNames()[k])
				{
					std::wstring animNames = mdl->GetAnimNames()[k];
					std::filesystem::path modelName(animNames);
					aJson[num]["AnimationPaths"][std::to_string(id)] = modelName;
					id++;
				}
			}
		}
		else
		{
			aJson[num]["IsAnim"] = false;
		}
		entt::entity& entity = aScene->GetEntitys(ObjectType::Model)[i];
		SaveComponents(aJson, num, entity, aScene);
		aNum++;
	}
}

void Editor::LoadModels(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum)
{
	entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
	entt::entity entity = reg.create();

	LoadComponents(aJson, aNum, entity, aScene);

	if (reg.all_of<ModelComponent, TransformComponent>(entity))
	{
		bool isAnim = aJson[aNum]["IsAnim"];
		std::string modelPath = aJson[aNum]["ModelPath"];
		std::filesystem::path modelName(modelPath);
		std::string name = aJson[aNum]["Name"];
		std::filesystem::path nameOfModel(name);
		if (modelPath == "Cube")
		{
			reg.get<ModelComponent>(entity).myModel = ModelAssetHandler::CreateCube(L"Cube");
		}
		else if (!isAnim)
		{
			reg.get<ModelComponent>(entity).myModel = ModelAssetHandler::LoadModel(modelName);
		}
		else
		{
			std::string name = aJson[aNum]["AnimationPaths"]["0"];
			std::filesystem::path firstAnimName(name);
			reg.get<ModelComponent>(entity).myModel = ModelAssetHandler::LoadModelWithAnimation(modelName, firstAnimName);
			for (size_t i = 1; i < aJson[aNum]["AnimationPaths"].size(); i++)
			{
				name = aJson[aNum]["AnimationPaths"][std::to_string(i)];
				std::filesystem::path animName(name);
				ModelAssetHandler::LoadAnimation(modelName, animName);
			}
		}
		reg.get<ModelComponent>(entity).myModel->SetName(nameOfModel);
		reg.get<ModelComponent>(entity).myModel->SetTransform(reg.get<TransformComponent>(entity).myTransform);
		aScene->AddModelInstance(reg.get<ModelComponent>(entity).myModel, entity);
	}
}

void Editor::SaveTexts(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum)
{
	for (size_t i = 0; i < aScene->GetTexts().size(); i++)
	{
		std::string num = std::to_string(aNum);
		std::shared_ptr<Text> text = aScene->GetTexts()[i];
		std::filesystem::path test(text->GetText());
		aJson[num]["Text"] = test;
		aJson[num]["FontSize"] = text->GetFont()->Atlas.Size;
		aJson[num]["Is2D"] = text->GetIs2D();

		entt::entity& entity = aScene->GetEntitys(ObjectType::Text)[i];
		SaveComponents(aJson, num, entity, aScene);
		aNum++;
	}
}

void Editor::LoadTexts(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum)
{
	entt::entity entity = aScene->GetRegistry().create();
	LoadComponents(aJson, aNum, entity, aScene);
	std::filesystem::path textData = aJson[aNum]["Text"];
	std::shared_ptr<Text> text = TextFactory::CreateText(textData, 1, aJson[aNum]["FontSize"], aJson[aNum]["Is2D"]);
	text->SetTransform(aScene->GetRegistry().get<TransformComponent>(entity).myTransform);
	aScene->AddText(text, entity);
}

void Editor::SaveParticleSystems(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum)
{
	for (size_t i = 0; i < aScene->GetParticleSystems().size(); i++)
	{
		std::string num = std::to_string(aNum);
		std::shared_ptr<ParticleSystem> system = aScene->GetParticleSystems()[i];
		EmitterSettingsData settings = system->GetEmitters()[0].GetEmitterSettings();
		aJson[num]["SpawnRate"] = settings.SpawnRate;
		aJson[num]["LifeTime"] = settings.LifeTime;
		aJson[num]["StartVelocity"] = { settings.StartVelocity.x, settings.StartVelocity.y, settings.StartVelocity.z };
		aJson[num]["EndVelocity"] = { settings.EndVelocity.x, settings.EndVelocity.y, settings.EndVelocity.z };
		aJson[num]["GravityScale"] = settings.GravityScale;
		aJson[num]["StartSize"] = settings.StartSize;
		aJson[num]["EndSize"] = settings.EndSize;
		aJson[num]["StartColor"] = { settings.StartColor.x, settings.StartColor.y, settings.StartColor.z, settings.StartColor.w };
		aJson[num]["EndColor"] = { settings.EndColor.x, settings.EndColor.y, settings.EndColor.z, settings.EndColor.w };
		aJson[num]["Looping"] = settings.Looping;
		aJson[num]["HasDuration"] = settings.HasDuration;
		aJson[num]["Duration"] = settings.Duration;

		entt::entity& entity = aScene->GetEntitys(ObjectType::ParticleSystem)[i];
		SaveComponents(aJson, num, entity, aScene);
		aNum++;
	}
}

void Editor::LoadParticleSystems(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum)
{
	entt::entity entity = aScene->GetRegistry().create();
	LoadComponents(aJson, aNum, entity, aScene);
	EmitterSettingsData data;

	data.SpawnRate = aJson[aNum]["SpawnRate"];
	data.LifeTime = aJson[aNum]["LifeTime"];

	data.StartVelocity.x = aJson[aNum]["StartVelocity"][0];
	data.StartVelocity.y = aJson[aNum]["StartVelocity"][1];
	data.StartVelocity.z = aJson[aNum]["StartVelocity"][2];

	data.EndVelocity.x = aJson[aNum]["EndVelocity"][0];
	data.EndVelocity.y = aJson[aNum]["EndVelocity"][1];
	data.EndVelocity.z = aJson[aNum]["EndVelocity"][2];

	data.GravityScale = aJson[aNum]["GravityScale"];
	data.StartSize = aJson[aNum]["StartSize"];
	data.EndSize = aJson[aNum]["EndSize"];

	data.StartColor.x = aJson[aNum]["StartColor"][0];
	data.StartColor.y = aJson[aNum]["StartColor"][1];
	data.StartColor.z = aJson[aNum]["StartColor"][2];
	data.StartColor.w = aJson[aNum]["StartColor"][3];

	data.EndColor.x = aJson[aNum]["EndColor"][0];
	data.EndColor.y = aJson[aNum]["EndColor"][1];
	data.EndColor.z = aJson[aNum]["EndColor"][2];
	data.EndColor.w = aJson[aNum]["EndColor"][3];

	data.Looping = aJson[aNum]["Looping"];
	data.HasDuration = aJson[aNum]["HasDuration"];
	data.Duration = aJson[aNum]["Duration"];

	std::shared_ptr<ParticleSystem> system = ParticleAssetHandler::CreateParticleSystem(data);
	SceneHandler::GetActiveScene()->AddParticleSystem(system, entity);
}

void Editor::SaveSettings()
{
	using nlohmann::json;
	const std::string path = "./Json/Settings/";
	const std::string fileName = "Settings.json";
	bool isFileFound = false;
	for (const auto& file : directory_iterator(path))
	{
		if (fileName == file.path().filename().string())
		{
			isFileFound = true;
			break;
		}
	}
	if (!isFileFound)
	{
		//std::ofstream file(sceneName);
		std::cout << "File Not Found" << std::endl;
	}
	else
	{
		std::cout << "File Found" << std::endl;
	}
	json j;
	j["CameraSpeed"] = SceneHandler::GetActiveScene()->GetCamera()->GetCameraSpeed();
	j["TimeScale"] = CommonUtilities::Timer::GetTimeScale();
	std::array<float, 4> color = GraphicsEngine::GetClearColor();
	j["ClearColor"] = { color[0], color[1], color[2], color[3] };
	j["PresetName"] = GraphicsEngine::myCurrentClearColorPreset;
	j["AutoSave"] = GraphicsEngine::GetAutoSave();
	std::ofstream oStream(path + fileName);
	oStream << j;
}

void Editor::LoadSettings()
{
	using nlohmann::json;
	json j;
	std::ifstream ifStream(L"Json/Settings/Settings.json");
	if (ifStream.fail())
		return;
	ifStream >> j;

	SceneHandler::GetActiveScene()->GetCamera()->GetCameraSpeed() = j["CameraSpeed"];
	CommonUtilities::Timer::SetTimeScale(j["TimeScale"]);
	std::array<float, 4> color = j["ClearColor"];
	GraphicsEngine::GetAutoSave() = j["AutoSave"];
	GraphicsEngine::GetClearColor() = color;
	LoadClearColorPreset(j["PresetName"]);
}

void Editor::SaveClearColorPreset(std::string aName)
{
	using nlohmann::json;
	const std::string path = "./Json/Settings/";
	const std::string fileName = aName + ".json";
	bool isFileFound = false;
	for (const auto& file : directory_iterator(path))
	{
		if (fileName == file.path().filename().string())
		{
			isFileFound = true;
			break;
		}
	}
	if (!isFileFound)
	{
		//std::ofstream file(sceneName);
		std::cout << "File Not Found" << std::endl;
	}
	else
	{
		std::cout << "File Found" << std::endl;
	}
	json j;
	j["PresetOne"] = { GraphicsEngine::myClearColorPresets[0][0],GraphicsEngine::myClearColorPresets[0][1],
		GraphicsEngine::myClearColorPresets[0][2], GraphicsEngine::myClearColorPresets[0][3] };
	j["PresetTwo"] = { GraphicsEngine::myClearColorPresets[1][0],GraphicsEngine::myClearColorPresets[1][1],
		GraphicsEngine::myClearColorPresets[1][2], GraphicsEngine::myClearColorPresets[1][3] };
	j["Factor"] = GraphicsEngine::myClearColorBlendFactor;
	j["IsBlending"] = GraphicsEngine::myClearColorBlending;
	GraphicsEngine::myCurrentClearColorPreset = aName;
	std::ofstream oStream(path + fileName);
	oStream << j;
}

void Editor::LoadClearColorPreset(std::string aName)
{
	using nlohmann::json;
	json j;
	std::ifstream ifStream("Json/Settings/" + aName + ".json");
	if (ifStream.fail())
	{
		GraphicsEngine::myClearColorBlendFactor = 0;
		GraphicsEngine::myClearColorBlending = false;
		std::array<float, 4> color = { 0,0,0,1 };
		GraphicsEngine::myClearColorPresets[0] = color;
		GraphicsEngine::myClearColorPresets[1] = color;
		GraphicsEngine::myCurrentClearColorPreset = "default";
		GraphicsEngine::GetClearColor() = color;
		return;
	}
	ifStream >> j;

	GraphicsEngine::myClearColorBlendFactor = j["Factor"];
	GraphicsEngine::myClearColorBlending = j["IsBlending"];
	std::array<float, 4> color = j["PresetOne"];
	GraphicsEngine::myClearColorPresets[0] = color;
	color = j["PresetTwo"];
	GraphicsEngine::myClearColorPresets[1] = color;
	GraphicsEngine::myCurrentClearColorPreset = aName;
}