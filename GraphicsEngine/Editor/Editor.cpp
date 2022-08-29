#include "GraphicsEngine.pch.h"
#include "Editor.h"
#include "../Model/ModelInstance.h"
#include "../Tools/InputHandler.h"
#include "../GraphicsEngine.h"
#include <filesystem>
#include <fstream>
#include "../GraphicsEngine/Tools/json.hpp"
#include "../Scene/SceneHandler.h"
#include "../Tools/Timer.h"


#include "../Engine/ComponentHandler.h"
#include "../Engine/Player.h"
//#include "entt/entt.hpp"

using std::filesystem::directory_iterator;

std::vector<Editor::EditorActions> Editor::myEditorActions;

void Editor::AddEditorAction(const EditorActions& anAction)
{
	myEditorActions.push_back(anAction);
	if (myEditorActions.size() >= 15)
		myEditorActions.erase(myEditorActions.begin() + 0);
}

void Editor::EditorActionHandler()
{
	if (CommonUtilities::InputHandler::GetKeyIsHeld(VK_CONTROL) && CommonUtilities::InputHandler::GetKeyIsPressed('Z'))
	{
		if (myEditorActions.size() > 0)
		{
			EditorActions action = myEditorActions[myEditorActions.size() - 1];
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
				action.Object->SetName(action.OldName);
			}
			else if (action.MovedObject)
			{
				action.Object->SetTransform(action.OldTransform);
			}
			else if (action.RemovedObject)
			{
				entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
				if (reg.any_of<ModelComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddModelInstance(reg.get<ModelComponent>(action.oldEntity).myModel, &action.oldEntity);
				if (reg.any_of<ParticleSystemComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddParticleSystem(reg.get<ParticleSystemComponent>(action.oldEntity).myParticleSystem, &action.oldEntity);
				if (reg.any_of<TextComponent>(action.oldEntity))
					SceneHandler::GetActiveScene()->AddText(reg.get<TextComponent>(action.oldEntity).myText, &action.oldEntity);
			}
			myEditorActions.erase(myEditorActions.begin() + myEditorActions.size() - 1);
		}
	}
}

void Editor::SaveModels()
{
	using nlohmann::json;
	const std::string path = _SOLUTIONDIR"Bin/Json/Scenes/";
	for (size_t i = 0; i < SceneHandler::GetScenes().size(); i++)
	{
		bool isFileFound = false;
		std::filesystem::path sceneName(SceneHandler::GetScenes()[i]->GetSceneName() + L".json");
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
		std::shared_ptr<Scene> scene = SceneHandler::GetScenes()[i];
		const int size = scene->GetModels().size();
		json j;
		j["SceneName"] = sceneName.string();
		j["Size"] = size;
		for (size_t i = 0; i < size; i++)
		{
			std::shared_ptr<ModelInstance> mdl = scene->GetModels()[i];
			std::filesystem::path path(mdl->GetPath());
			std::filesystem::path name(mdl->GetName());
			std::string num = std::to_string(i);
			j[num]["ModelPath"] = path;
			j[num]["Name"] = name;
			j[num]["Position"] = { mdl->GetTransform().GetPosition().x, mdl->GetTransform().GetPosition().y, mdl->GetTransform().GetPosition().z };
			j[num]["Scale"] = { mdl->GetTransform().GetScale().x, mdl->GetTransform().GetScale().y, mdl->GetTransform().GetScale().z };
			j[num]["Rotation"] = { mdl->GetTransform().GetRotation().x, mdl->GetTransform().GetRotation().y, mdl->GetTransform().GetRotation().z };
			j[num]["IsAnim"] = mdl->HasBones();

			entt::registry& reg = scene->GetRegistry();
			std::vector<entt::entity>& entitys = scene->GetEntitys();
			std::vector<int> ids;

			if (reg.any_of<ModelComponent>(entitys[i]))
			{
				ids.push_back(0);
			}
			if (reg.any_of<PlayerComponent>(entitys[i]))
			{
				ids.push_back(1);
			}
			if (reg.any_of<TransformComponent>(entitys[i]))
			{
				ids.push_back(2);
			}
			if (reg.any_of<ParticleSystemComponent>(entitys[i]))
			{
				ids.push_back(3);
			}
			if (reg.any_of<TextComponent>(entitys[i]))
			{
				ids.push_back(4);
			}
			for (size_t k = 0; k < ids.size(); k++)
			{
				int id = ids[k];
				j[num]["Components"][std::to_string(k)] = id;
			}

			if (mdl->HasBones())
			{
				std::filesystem::path animName(mdl->GetCurrentAnimation().myName);
				j[num]["AnimationPaths"]["0"] = animName;
				//mdl->RemoveAnimName(mdl->GetCurrentAnimation().myName);
				for (size_t k = 0; k < mdl->GetAnimNames().size(); k++)
				{
					std::wstring animNames = mdl->GetAnimNames()[k];
					std::filesystem::path modelName(animNames);
					j[num]["AnimationPaths"][std::to_string(k + 1)] = modelName;
				}
			}
		}
		std::ofstream oStream(path + sceneName.string());
		oStream << j;
	}
}

void Editor::LoadModels()
{
	using nlohmann::json;
	std::shared_ptr<Scene> scene = SceneHandler::GetActiveScene();
	json j;
	std::ifstream ifStream(L"Json/Scenes/" + scene->GetSceneName() + L".json");
	if (ifStream.fail())
		return;
	ifStream >> j;
	for (size_t i = 0; i < j["Size"]; i++)
	{
		std::string num = std::to_string(i);

		entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
		entt::entity entity = reg.create();
		for (size_t k = 0; k < j[num]["Components"].size(); k++)
		{
			int compId = j[num]["Components"][std::to_string(k)];
			ComponentHandler::AddComponent(compId, entity);
		}


		std::string name = j[num]["Name"];
		if (reg.any_of<TransformComponent>(entity))
		{
			Vector3f position = { j[num]["Position"][0], j[num]["Position"][1], j[num]["Position"][2] };
			Vector3f scale = { j[num]["Scale"][0], j[num]["Scale"][1], j[num]["Scale"][2] };
			Vector3f rotation = { j[num]["Rotation"][0], j[num]["Rotation"][1], j[num]["Rotation"][2] };

			if (reg.any_of<ModelComponent>(entity))
			{
				std::string modelPath = j[num]["ModelPath"];
				bool isAnim = j[num]["IsAnim"];
				std::filesystem::path modelName(modelPath);
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
					std::string name = j[num]["AnimationPaths"]["0"];
					std::filesystem::path firstAnimName(name);
					reg.get<ModelComponent>(entity).myModel = ModelAssetHandler::LoadModelWithAnimation(modelName, firstAnimName);
					for (size_t i = 1; i < j[num]["AnimationPaths"].size(); i++)
					{
						name = j[num]["AnimationPaths"][std::to_string(i)];
						std::filesystem::path animName(name);
						ModelAssetHandler::LoadAnimation(modelName, animName);
					}
				}
				reg.get<TransformComponent>(entity).myTransform.SetPosition(position);
				reg.get<TransformComponent>(entity).myTransform.SetScale(scale);
				reg.get<TransformComponent>(entity).myTransform.SetRotation(rotation);
				reg.get<ModelComponent>(entity).myModel->SetName(nameOfModel);
				reg.get<ModelComponent>(entity).myModel->SetTransform(reg.get<TransformComponent>(entity).myTransform);
				scene->AddModelInstance(reg.get<ModelComponent>(entity).myModel, &entity);
			}
			if (reg.any_of<ParticleSystemComponent>(entity))
			{

			}
		}









		//std::shared_ptr<ModelInstance> mdl;
		//std::filesystem::path modelName(modelPath);
		//std::filesystem::path nameOfModel(name);
		//if (modelPath == "Cube")
		//{
		//	mdl = ModelAssetHandler::CreateCube(L"Cube");
		//}
		//else if (!isAnim)
		//{
		//	mdl = ModelAssetHandler::LoadModel(modelName);
		//}
		//else
		//{
		//	std::string name = j[num]["AnimationPaths"]["0"];
		//	std::filesystem::path firstAnimName(name);
		//	mdl = ModelAssetHandler::LoadModelWithAnimation(modelName, firstAnimName);
		//	for (size_t i = 1; i < j[num]["AnimationPaths"].size(); i++)
		//	{
		//		name = j[num]["AnimationPaths"][std::to_string(i)];
		//		std::filesystem::path animName(name);
		//		ModelAssetHandler::LoadAnimation(modelName, animName);
		//	}
		//}


		////ENTT TEST
		//static int id = 0;
		//if (id == 2)
		//{
		//	entt::entity ent = SceneHandler::GetActiveScene()->GetRegistry().create();
		//	Transform trans;
		//	trans.SetPosition(position);
		//	trans.SetScale(scale);
		//	trans.SetRotation(rotation);
		//	//Player player;
		//	//player.Init(trans, ent);
		//	//ComponentHandler::AddComponent<Player>(ent);
		//	SceneHandler::GetActiveScene()->GetRegistry().emplace<Player>(ent);
		//	SceneHandler::GetActiveScene()->GetRegistry().get<Player>(ent).Init(position, rotation, scale, ent);
		//}

		//mdl->SetPosition(position);
		//mdl->SetScale(scale);
		//mdl->SetRotation(rotation);
		//mdl->SetName(nameOfModel);
		//scene->AddGameObject(mdl);
		//id++;
	}
}

void Editor::SaveSettings()
{
	using nlohmann::json;
	const std::string path = _SOLUTIONDIR"Bin/Json/Settings/";
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
}