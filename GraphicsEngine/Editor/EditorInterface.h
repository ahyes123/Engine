#pragma once
#include "../Math/Matrix4x4.hpp"
#include <filesystem>

class Scene;
class SceneObject;
class Entity;

using std::filesystem::directory_iterator;
class EditorInterface
{
private:
	static std::filesystem::path myCurrentPath;
public:
	static void ShowEditor();
	static std::filesystem::path GetCurrentPath() { return myCurrentPath; }
	static bool IsInsideViewPort(Vector2f aPos);
	static bool IsInsideAssetBrowser(Vector2f aPos);
private:
	static void SceneHierchy(std::shared_ptr<Scene> aScene);
	static void EnableDocking();
	static void MenuBar();
	static void Properties(std::shared_ptr<Entity> aEntity);
	static void AssetBrowser();

	static inline std::shared_ptr<Entity> selectedEntity;
	static inline Vector2f viewPortSize;
	static inline Vector2f viewPortPos;
	static inline Vector2f assetBrowserSize;
	static inline Vector2f assetBrowserPos;

	static bool EditorGuizmo(std::shared_ptr<Entity> aEntity);
	static void DragAndDropHierchy(const int& aIndex);
	static void DragAndDrop(std::shared_ptr<SceneObject>& aObject, std::vector<std::shared_ptr<SceneObject>>& aObjectVector, bool& aAcceptedDragDrop);
};