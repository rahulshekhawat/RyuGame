#pragma once

#include "RyuLib.h"

class FSceneManager;
class GLFWwindow;

class RYU_API FRyuEngine
{

public:

	FRyuEngine();
	~FRyuEngine();


	static void RegisterGameWindow(GLFWwindow* Window) { FRyuEngine::Window = Window; }
	static void RegisterSceneManager(FSceneManager* SceneManager) { FRyuEngine::SceneManager = SceneManager; }

private:

	static FSceneManager* SceneManager;
	static GLFWwindow* Window;

};
