#pragma once
#include <memory>
#include "../GraphicsEngine/Scene/Transform.h"
#include "../External/entt/entity/entity.hpp"
#include "../Scene/SceneObject.h"
#include "../Tools/json.hpp"
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
		std::shared_ptr<SceneObject> Object;
		Transform OldTransform;
		std::wstring OldName;
		entt::entity oldEntity;
	};
	static void AddEditorAction(const EditorActions& anAction);
	static void EditorActionHandler();
	static void SaveScenes();
	static void LoadScenes();
	static void SaveComponents(nlohmann::json& aJson, std::string& aNum, entt::entity aEntity, std::shared_ptr<Scene> aScene);
	static void SaveModels(std::shared_ptr<Scene> aScene, nlohmann::json& aJson, std::string& aNum);
	static void LoadModels();
	static void SaveTexts(nlohmann::json& aJson);
	static void LoadTexts();
	static void SaveSettings();
	static void LoadSettings();

private:
	static std::vector<EditorActions> myEditorActions;
};