#pragma once

#define GLFW_INCLUDE_VULKAN

#include <Windows.h>
#include <GLFW/glfw3.h>
#include "RyuLib.h"


class RYU_API FApplicationBase
{
public:

	FApplicationBase();
	virtual ~FApplicationBase();

	virtual bool Create();
	virtual int Run();
	virtual int MainLoop();

	virtual void Destroy();

protected:

	virtual bool InitWindow();
	virtual bool InitVulkan();

	GLFWwindow* Window;

private:

	void CreateApplicationInfo();

	DWORD MainThreadID;
	DWORD MainProcessID;

};
