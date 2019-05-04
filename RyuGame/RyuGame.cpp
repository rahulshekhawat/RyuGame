#include "RyuGame.h"

FRyuGame::FRyuGame()
{
}

FRyuGame::~FRyuGame()
{
}

bool FRyuGame::InitWindow()
{
    if (Window != nullptr)
    {
        rlog("Window already exists\n");
        return false;
    }

	int initResult = glfwInit();
	if (initResult == GLFW_FALSE)
	{
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	Window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "RyuGame", nullptr, nullptr);
	if (Window == nullptr)
	{
		return false;
	}

	return true;
}

bool FRyuGame::InitVulkan()
{
	return FApplicationBase::InitVulkan();
}
