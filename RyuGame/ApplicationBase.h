#pragma once

#define GLFW_INCLUDE_VULKAN

#include <Windows.h>
#include <GLFW/glfw3.h>
#include "RyuLib.h"

#include <stdexcept>
#include <vector>
#include <optional>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value();
	}
};


class RYU_API FApplicationBase
{
public:

	FApplicationBase();
	virtual ~FApplicationBase();

	virtual void Update();
	virtual void Render();

	virtual bool Create();
	virtual int Run();
	virtual int MainLoop();
	virtual void Destroy();

protected:

	virtual bool InitWindow();
	virtual bool InitVulkan();

	GLFWwindow* Window;

private:

	std::vector<const char*> GetRequiredExtensions();
	void CreateApplicationInfo();
	bool CreateVulkanInstance();
	bool PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	DWORD MainThreadID;
	DWORD MainProcessID;

	bool bIsActive;

	VkInstance VulkanInstance;
	VkPhysicalDevice PhysicalDevice;

};
