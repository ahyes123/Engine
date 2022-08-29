#pragma once
#include "../Math/Matrix4x4.hpp"

class EditorInterface
{
public:
	static void SceneHierchy();
	static void ModelLoader();
private:
	static bool addAnimation;
	static int selectedItem;
	static bool EditorGuizmo(Matrix4x4f& aObjectMatrix);
};