#include "ApplicationBase.h"

UApplicationBase::UApplicationBase() : Window(nullptr), MainThreadID(NULL), MainProcessID(NULL)
{
}

UApplicationBase::~UApplicationBase()
{
}

bool UApplicationBase::Create()
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

int UApplicationBase::Run()
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

int UApplicationBase::MainLoop()
{
	rlog("Entering Mainloop\n");

	while (!glfwWindowShouldClose(Window))
	{
		Sleep(100);
		glfwPollEvents();
	}

	return GLFW_TRUE;
}

void UApplicationBase::Destroy()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

bool UApplicationBase::InitWindow()
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

bool UApplicationBase::InitVulkan()
{
	return true;
}

void UApplicationBase::CreateApplicationInfo()
{
	MainThreadID = GetCurrentThreadId();
	MainProcessID = GetCurrentProcessId();
}
