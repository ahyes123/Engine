#pragma once
#include "../Math/Matrix4x4.hpp"
#include "entt/entity/entity.hpp"

class Scene;

class EditorInterface
{
public:
	static void ShowEditor();
private:
	static void SceneHierchy(bool aSomeThingSelected, std::shared_ptr<Scene> aScene);
	static void ModelLoader();
	static void EnableDocking();
	static void MenuBar();
	static void Properties(bool aSomeThingSelected, std::shared_ptr<Scene> aScene);
	static void AddComponentTab(bool aSomeThingSelected, std::shared_ptr<Scene> aScene);
	static bool addAnimation;
	static int selectedItem;
	static bool EditorGuizmo(entt::entity aObjectMatrix);
};