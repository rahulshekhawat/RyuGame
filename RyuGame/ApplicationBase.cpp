#include "ApplicationBase.h"
#include <algorithm>
#include <limits>
#include <set>
#include <string>

FApplicationBase::FApplicationBase() :
	Window(nullptr),
	MainThreadID(NULL),
	MainProcessID(NULL),
	bIsActive(false)
{
}

FApplicationBase::~FApplicationBase()
{
}

void FApplicationBase::Update()
{
}

void FApplicationBase::Render()
{
}

bool FApplicationBase::CreateAppDisplay()
{
	CreateApplicationInfo();

	if (!InitWindow())
	{
		rlog_warn("failed to initialize window\n");
		return false;
	}

	if (!InitVulkan())
	{
		rlog_warn("failed to initialize vulkan\n");
		return false;
	}

	return true;
}

int FApplicationBase::Run()
{
	rlog("application started\n");

	if (!CreateAppDisplay())
	{
		rlog_err("application display failed to create\n");
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
		if (!bIsActive)
		{
			Sleep(10);
		}

		glfwPollEvents();
	}

	return GLFW_TRUE;
}

void FApplicationBase::Destroy()
{
	vkDestroyDevice(LogicalDevice, nullptr);
	vkDestroySurfaceKHR(VulkanInstance, VulkanSurface, nullptr);
	vkDestroyInstance(VulkanInstance, nullptr);

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
	if (!CreateVulkanInstance())
	{
		rlog_warn("failed to create vulkan instance\n");
		return false;
	}

	if (!CreateSurface())
	{
		rlog_warn("failed to create vulkan surface\n");
		return false;
	}

	if (!PickPhysicalDevice())
	{
		rlog_warn("failed to pick physical device\n");
		return false;
	}
	
	if (!CreateLogicalDevice())
	{
		rlog_warn("failed to create logical device\n");
		return false;
	}

	if (!CreateSwapChain())
	{
		rlog_warn("failed to create swap chain buffer\n");
		return false;
	}

	if (!CreateImageViews())
	{
		rlog_warn("failed to create image views\n");
		return false;
	}

	return true;
}

std::vector<const char*> FApplicationBase::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	/**
	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	*/

	return extensions;
}

void FApplicationBase::CreateApplicationInfo()
{
	MainThreadID = GetCurrentThreadId();
	MainProcessID = GetCurrentProcessId();
}

bool FApplicationBase::CreateVulkanInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "RyuGame";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "RyuEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &VulkanInstance) != VK_SUCCESS)
	{
		rlog_err("failed to create instance!\n");
		return false;
	}

	return true;
}

bool FApplicationBase::CreateSurface()
{
	if (glfwCreateWindowSurface(VulkanInstance, Window, nullptr, &VulkanSurface) != VK_SUCCESS)
	{
		rlog_err("failed to create vulkan surface!\n");
		return false;
	}

	return true;
}

bool FApplicationBase::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		rlog_err("failed to find GPUs with vulkan support!\n");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			PhysicalDevice = device;
			break;
		}
	}

	if (PhysicalDevice == VK_NULL_HANDLE)
	{
		rlog_err("failed to find a suitable GPU!\n");
		return false;
	}

	return true;
}

bool FApplicationBase::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &LogicalDevice) != VK_SUCCESS)
	{
		rlog_err("failed to create logical device!\n");
		return false;
	}

	vkGetDeviceQueue(LogicalDevice, indices.graphicsFamily.value(), 0, &GraphicsQueue);
	return true;
}

bool FApplicationBase::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = VulkanSurface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(LogicalDevice, &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
	{
		rlog_err("failed to create swap chain!\n");
		return false;
	}

	vkGetSwapchainImagesKHR(LogicalDevice, SwapChain, &imageCount, nullptr);
	SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(LogicalDevice, SwapChain, &imageCount, SwapChainImages.data());

	SwapChainImageFormat = surfaceFormat.format;
	SwapChainExtent = extent;

	return true;
}

bool FApplicationBase::CreateImageViews()
{
	SwapChainImageViews.resize(SwapChainImages.size());

	for (size_t i = 0; i < SwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(LogicalDevice, &createInfo, nullptr, &SwapChainImageViews[i]) != VK_SUCCESS)
		{
			rlog_err("failed to create image views!\n")
			return false;
		}
	}

	return true;
}

bool FApplicationBase::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool FApplicationBase::CheckDeviceExtensionSupport(VkPhysicalDevice PhysicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices FApplicationBase::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VulkanSurface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails FApplicationBase::QuerySwapChainSupport(VkPhysicalDevice PhysicalDevice)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, VulkanSurface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, VulkanSurface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, VulkanSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, VulkanSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, VulkanSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR FApplicationBase::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
{
	if (AvailableFormats.size() == 1 && AvailableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& AvailableFormat : AvailableFormats)
	{
		if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return AvailableFormat;
		}
	}

	return AvailableFormats[0];
}

VkPresentModeKHR FApplicationBase::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : AvailablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D FApplicationBase::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities)
{
	if (Capabilities.currentExtent.width != UINT32_MAX)
	{
		return Capabilities.currentExtent;
	}
	else
	{
		VkExtent2D ActualExtent = { DEFAULT_WIDTH, DEFAULT_HEIGHT };

		std::min<uint32_t>(Capabilities.maxImageExtent.width, ActualExtent.width);

		ActualExtent.width = std::max<uint32_t>(Capabilities.minImageExtent.width, std::min<uint32_t>(Capabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = std::max<uint32_t>(Capabilities.minImageExtent.height, std::min<uint32_t>(Capabilities.maxImageExtent.height, ActualExtent.height));

		return ActualExtent;
	}
}
