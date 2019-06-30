#pragma once

#define GLFW_INCLUDE_VULKAN

#include "RyuLogger.h"
#include "RyuLib.h"

#include <Windows.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <optional>
#include <string>

struct FAppInfo
{
	std::string Name;
	std::string Version;
	std::string Author;
	std::string Type;
	std::string Description;

	FAppInfo() :
		Name("RyuGame"),
		Version("0.0.1"),
		Author("Rahul Shekhawat"),
		Type("Game"),
		Description("")
	{
	}
};

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

	static FAppInfo AppInfo;

protected:

	virtual bool InitWindow();
	virtual bool InitVulkan();

	static void OnWindowResized(GLFWwindow* Window, int NewWidth, int NewHeight);

	GLFWwindow* Window;

private:

	//~ Begin Vulkan Interface
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
	//~ End Vulkan Interface
};
