#pragma once

#include "SceneHandler.h"
#include "../GraphicsEngine/Scene/Transform.h"
#include "../External/entt/entity/entity.hpp"
#include "../Scene/SceneObject.h"
#include "../External/nloman/json.hpp"
#include "../Scene/Scene.h"
class ModelInstance;

class Editor
{
public:
	struct EditorActions
	{
		bool ChangedName = false;
		bool AddedObject = false;
		bool RemovedObject = false;
		bool MovedObject = false;
		std::shared_ptr<SceneObject> Object = nullptr;
		Transform OldTransform;
		Transform beforeTransform;
		std::wstring OldName = L"";
		std::wstring beforeName = L"";
		entt::entity oldEntity = SceneHandler::GetActiveScene()->GetRegistry().create();
	};
	static void AddUndoAction(const EditorActions& anAction, const bool& aNewAction = true);
	static void AddRedoActoin(const EditorActions& anAction);
	static void EditorActionHandler();
	static void SaveCurrentScene();
	static void SaveScenes();
	static void LoadCurrentScene();
	static void SaveSettings();
	static void LoadSettings();
	static void SaveClearColorPreset(std::string aName);
	static void LoadClearColorPreset(std::string aName);

private:
	static std::vector<EditorActions> myUndoActions;
	static std::vector<EditorActions> myRedoActions;

	static void SaveModels(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum);
	static void LoadModels(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum);
	static void SaveTexts(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum);
	static void LoadTexts(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum);
	static void SaveParticleSystems(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum);
	static void LoadParticleSystems(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string aNum);
	static void SaveComponents(nlohmann::json& aJson, std::string& aNum, entt::entity aEntity, std::shared_ptr<Scene> aScene);
	static void LoadComponents(nlohmann::json& aJson, std::string& aNum, entt::entity aEntity, std::shared_ptr<Scene> aScene);
	static void SaveParents(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum);
	static void LoadParents(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, int& aNum);
};