#pragma once
#include "../Math/Matrix4x4.hpp"
#include "entt/entity/entity.hpp"

class Scene;
class SceneObject;

class EditorInterface
{
public:
	static void ShowEditor();
	static void SetTexture(std::wstring aFilePath);
private:
	static void SceneHierchy(std::shared_ptr<Scene> aScene);
	static void ModelLoader();
	static void EnableDocking();
	static void MenuBar();
	static void Properties(std::shared_ptr<Scene> aScene);
	static void AddComponentTab(std::shared_ptr<Scene> aScene);
	static bool addAnimation;
	static bool someSelected;
	static int selectedItem;
	static entt::entity selectedEntity;
	static bool EditorGuizmo(entt::entity aObjectMatrix);
	static void DragAndDropHierchy(const int& aIndex);
	static void ShowObjectChildren(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop);
	static void DragAndDrop(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop);
	static bool HasConnection(std::shared_ptr<SceneObject> aFirstObject, std::shared_ptr<SceneObject> aSecondObject);
};