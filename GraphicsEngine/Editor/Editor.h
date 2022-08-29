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
	static void SaveModels();
	static void LoadModels();
	static void SaveTexts(nlohmann::json& aJson);
	static void LoadTexts();
	static void SaveSettings();
	static void LoadSettings();

private:
	static std::vector<EditorActions> myEditorActions;
};