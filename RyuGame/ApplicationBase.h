#pragma once

#define GLFW_INCLUDE_VULKAN

#include <Windows.h>
#include <GLFW/glfw3.h>
#include "RyuLib.h"

#include <stdexcept>
#include <vector>
#include <optional>

const int DEFAULT_WIDTH = 1280;
const int DEFAULT_HEIGHT = 720;

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class RYU_API FApplicationBase
{
public:

	FApplicationBase();
	virtual ~FApplicationBase();

	virtual void Update();
	virtual void Render();

	virtual bool CreateAppDisplay();
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
	bool CreateSurface();
	bool PickPhysicalDevice();
	bool CreateLogicalDevice();
	bool CreateSwapChain();
	bool CreateImageViews();
	bool IsDeviceSuitable(VkPhysicalDevice device);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice PhysicalDevice);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice PhysicalDevice);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities);

	DWORD MainThreadID;
	DWORD MainProcessID;

	bool bIsActive;

	VkInstance VulkanInstance;
	VkPhysicalDevice PhysicalDevice;
	VkDevice LogicalDevice;
	VkQueue GraphicsQueue;
	VkSurfaceKHR VulkanSurface;
	VkSwapchainKHR SwapChain;
	std::vector<VkImage> SwapChainImages;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;


};
