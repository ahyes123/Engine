#pragma once
#include "Scene.h"

class SceneHandler
{
public:
	static std::shared_ptr<Scene> GetActiveScene();
	static std::vector<std::shared_ptr<Scene>> GetScenes();

	static std::shared_ptr<Scene> AddEmptyScene(const std::wstring& aSceneName);
	static void AddNewScene(const std::shared_ptr<Scene>& aScene);
	static void LoadScene(const std::shared_ptr<Scene>& aScene);
	static std::shared_ptr<Scene> LoadScene(const unsigned int& aSceneIndex);
	static std::shared_ptr<Scene> LoadScene(const std::wstring& aSceneName);
	static int GetCurrentSceneID();
	static void UpdateCurrentScene();
	static void EmptyScene();
	static void LoadAllScenes();

private:
	static std::shared_ptr<Scene> myCurrentScene;
	static std::vector<std::shared_ptr<Scene>> myScenes;
};