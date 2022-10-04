#pragma once
#include "../Math/Matrix4x4.hpp"
#include "entt/entity/entity.hpp"
#include <filesystem>

class Scene;
class SceneObject;
using std::filesystem::directory_iterator;
class EditorInterface
{
private:
	static std::filesystem::path myCurrentPath;
public:
	static void ShowEditor();
	static void SetTexture(std::wstring aFilePath);
	static std::filesystem::path GetCurrentPath() { return myCurrentPath; }
	static bool IsInsideViewPort(Vector2f aPos);
	static bool IsInsideAssetBrowser(Vector2f aPos);
	static inline bool CANTDROPHERE = false;
private:
	static void SceneHierchy(std::shared_ptr<Scene> aScene);
	static void ModelLoader();
	static void EnableDocking();
	static void MenuBar();
	static void Properties(std::shared_ptr<Scene> aScene);
	static void AddComponentTab(std::shared_ptr<Scene> aScene);
	static void AssetBrowser();
	static bool addAnimation;
	static bool someSelected;
	static int selectedItem;
	static inline Vector2f viewPortSize;
	static inline Vector2f viewPortPos;
	static inline Vector2f assetBrowserSize;
	static inline Vector2f assetBrowserPos;
	static entt::entity selectedEntity;
	static bool EditorGuizmo(entt::entity aObjectMatrix);
	static void DragAndDropHierchy(const int& aIndex);
	static void ShowObjectChildren(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop);
	static void DragAndDrop(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop);
	static bool HasConnection(std::shared_ptr<SceneObject> aFirstObject, std::shared_ptr<SceneObject> aSecondObject);
};