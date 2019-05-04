#include "ApplicationBase.h"

FApplicationBase::FApplicationBase() : Window(nullptr), MainThreadID(NULL), MainProcessID(NULL)
{
}

FApplicationBase::~FApplicationBase()
{
}

bool FApplicationBase::Create()
{
	CreateApplicationInfo();

	if (!InitWindow())
	{
		return false;
	}

	if (!InitVulkan())
	{
		return false;
	}

	return true;
}

int FApplicationBase::Run()
{
	rlog("Application started\n");

	if (!Create())
	{
		rlog("Application failed to create\n");
		Destroy();
		return 0;
	}

	int LoopReturn = MainLoop();
	Destroy();

	return LoopReturn;
}

int FApplicationBase::MainLoop()
{
	rlog("Entering Mainloop\n");

	while (!glfwWindowShouldClose(Window))
	{
		Sleep(10);
		glfwPollEvents();
	}

	return GLFW_TRUE;
}

void FApplicationBase::Destroy()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

bool FApplicationBase::InitWindow()
{
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

bool FApplicationBase::InitVulkan()
{
	return true;
}

void FApplicationBase::CreateApplicationInfo()
{
	MainThreadID = GetCurrentThreadId();
	MainProcessID = GetCurrentProcessId();
}
