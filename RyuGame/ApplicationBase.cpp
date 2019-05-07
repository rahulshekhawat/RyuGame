#include "ApplicationBase.h"

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
		return false;
	}

	if (!PickPhysicalDevice())
	{
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
		rlog("failed to create instance!");
		return false;
	}
	else
	{
		return true;
	}
}

bool FApplicationBase::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(VulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		rlog("failed to find GPUs with vulkan support!");
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
		rlog("failed to find a suitable GPU!");
		return false;
	}

	return true;
}

bool FApplicationBase::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);
	return indices.isComplete();
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

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}
