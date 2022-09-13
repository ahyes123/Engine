#include "GraphicsEngine.pch.h"
#include "SceneHandler.h"
#include "../Tools/Timer.h"
#include "../Editor/Editor.h"
#include "../GraphicsEngine.h"
#include <filesystem>

std::shared_ptr<Scene> SceneHandler::myCurrentScene;
std::vector<std::shared_ptr<Scene>> SceneHandler::myScenes;

using std::filesystem::directory_iterator;

std::shared_ptr<Scene> SceneHandler::GetActiveScene()
{
	return myCurrentScene;
}

std::vector<std::shared_ptr<Scene>> SceneHandler::GetScenes()
{
	return myScenes;
}

std::shared_ptr<Scene> SceneHandler::AddEmptyScene(const std::wstring& aSceneName)
{
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	scene->SetSceneName(aSceneName);
	if (myScenes.size() == 0)
	{
		myCurrentScene = scene;
		myCurrentScene->SetCamera(GraphicsEngine::GetCamera());
		Editor::LoadCurrentScene();
	}
	myScenes.push_back(scene);
	return scene;
}

void SceneHandler::AddNewScene(const std::shared_ptr<Scene>& aScene)
{
	if (myScenes.size() == 0)
	{
		myCurrentScene = aScene;
		Editor::LoadCurrentScene();
	}
	myScenes.push_back(aScene);
}

void SceneHandler::LoadScene(const std::shared_ptr<Scene>& aScene)
{
	//if (myCurrentScene.get() != nullptr)
	//{
	//	EmptyScene();
	//}
	aScene->SetCamera(myCurrentScene->GetCamera());
	myCurrentScene = aScene;
	if (myCurrentScene->GetModels().size() == 0)
		Editor::LoadCurrentScene();
}

std::shared_ptr<Scene> SceneHandler::LoadScene(const unsigned int& aSceneIndex)
{
	//if (myCurrentScene.get() != nullptr)
	//{
	//	EmptyScene();
	//}
	myScenes[aSceneIndex]->SetCamera(myCurrentScene->GetCamera());
	myCurrentScene = myScenes[aSceneIndex];
	if (myCurrentScene->GetModels().size() == 0)
		Editor::LoadCurrentScene();
	return myCurrentScene;
}

std::shared_ptr<Scene> SceneHandler::LoadScene(const std::wstring& aSceneName)
{
	std::filesystem::path name(aSceneName);
	std::wstring realName = name.filename().replace_extension("").wstring();
	for (size_t i = 0; i < myScenes.size(); i++)
	{
		if (myScenes[i]->GetSceneName() == realName)
		{
			myScenes[i]->SetCamera(myCurrentScene->GetCamera());
			myCurrentScene = myScenes[i];
			if (myCurrentScene->GetModels().size() == 0)
				Editor::LoadCurrentScene();
			return myCurrentScene;
		}
	}
	const std::wstring scenePath = L"./Json/Scenes";
	const std::wstring scenePathFull = L"./Json/Scenes" + realName + L".json";
	for (const auto& file : std::filesystem::directory_iterator(scenePath))
	{
		if (file.path() == scenePathFull)
		{
			return AddEmptyScene(realName);
		}
	}
	return nullptr;
}

int SceneHandler::GetCurrentSceneID()
{
	int id = 0;
	for (size_t i = 0; i < myScenes.size(); i++)
	{
		if (myScenes[i] == myCurrentScene)
		{
			id = i;
			break;
		}
	}
	return id;
}

void SceneHandler::UpdateCurrentScene()
{
	myCurrentScene->Update(CommonUtilities::Timer::GetDeltaTime());
}

void SceneHandler::EmptyScene()
{
	for (size_t i = 0; i < myCurrentScene->GetModels().size(); i++)
	{
		myCurrentScene->RemoveAllGameObjects();
	}
}

void SceneHandler::LoadAllScenes()
{
	const std::string path = "./Json/Scenes/";
	for (const auto& file : directory_iterator(path))
	{
		std::wstring name = file.path().filename().wstring();
		name = name.erase(name.size() - 5);
		AddEmptyScene(name);
	}
}