#include <qubeengine/vulkan_tutorial/VulkanTutorial.h>
#include <stdexcept>
#include <iostream>

namespace qe
{
	const int VulkanTutorial::WINDOW_WIDTH = 800;
	const int VulkanTutorial::WINDOW_HEIGHT = 600;

#ifdef NDEBUG
	const bool VulkanTutorial::ENABLE_VAL_LAYERS = false;
#else
	const bool VulkanTutorial::ENABLE_VAL_LAYERS = true;
#endif

	VkResult VulkanTutorial::createDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void VulkanTutorial::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanTutorial::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	VulkanTutorial::VulkanTutorial() :
		mpWindow(nullptr),
		mVulkanInstance(nullptr),
		mValidationLayers(std::vector<const char*> { "VK_LAYER_KHRONOS_validation" })
	{}

	void VulkanTutorial::run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void VulkanTutorial::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		mpWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!", nullptr, nullptr);
	}

	void VulkanTutorial::initVulkan()
	{
		createInstance();
		setupDebugMessenger();
	}

	void VulkanTutorial::createInstance()
	{
		if (ENABLE_VAL_LAYERS && !checkValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, but are NOT available :(");
		else
			std::cout << "Validation layers requested are available!" << std::endl;

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Qube Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> requiredExtensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		if (ENABLE_VAL_LAYERS && !validateRequiredExtensions(requiredExtensions))
			throw std::runtime_error("Failed to load requried glfw extensions.");
		else
			std::cout << "Successfully loaded required glfw extensions!" << std::endl;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

		if (ENABLE_VAL_LAYERS)
		{
			createInfo.enabledLayerCount = static_cast<uint32>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vulkan instance.");
		else
			std::cout << "Successfully initialized Vulkan!" << std::endl;
	}

	void VulkanTutorial::setupDebugMessenger()
	{
		if (!ENABLE_VAL_LAYERS) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up debug messenger!");
		}
		else
		{
			std::cout << "Created DebugUtilsMessengerEXT" << std::endl;
		}
	}

	void VulkanTutorial::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}

	bool VulkanTutorial::validateRequiredExtensions(const std::vector<const char*>& requiredExtensions)
	{
		bool success = true;
		uint32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::cout << "\nRequired extensions:" << std::endl;
		for (auto i = 0; i < requiredExtensions.size(); ++i)
			std::cout << "\t" << requiredExtensions[i] << std::endl;

		std::cout << "\nAvailable extensions:" << std::endl;
		for (const auto& extension : availableExtensions)
			std::cout << "\t" << extension.extensionName << std::endl;

		for (const auto& requiredExtension : requiredExtensions)
		{
			bool isExtentionFound = false;
			for (const auto& loadedExtension : availableExtensions)
			{
				if (strcmp(requiredExtension, loadedExtension.extensionName) == 0)
				{
					isExtentionFound = true;
					break;
				}
			}

			if (!isExtentionFound)
			{
				success = false;
				break;
			}
		}

		return success;
	}

	bool VulkanTutorial::checkValidationLayerSupport()
	{
		uint32 layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		bool success = true;
		for (const char* layerName : mValidationLayers)
		{
			bool isLayerFound = false;
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					isLayerFound = true;
					break;
				}
			}

			if (!isLayerFound)
			{
				success = false;
				break;
			}
		}

		return success;
	}

	std::vector<const char*> VulkanTutorial::getRequiredExtensions()
	{
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (ENABLE_VAL_LAYERS)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void VulkanTutorial::mainLoop()
	{
		while (!glfwWindowShouldClose(mpWindow))
		{
			glfwPollEvents();
		}
	}

	void VulkanTutorial::cleanup()
	{
		if (ENABLE_VAL_LAYERS)
			destroyDebugUtilsMessengerEXT(mVulkanInstance, mDebugMessenger, nullptr);

		vkDestroyInstance(mVulkanInstance, nullptr);
		glfwDestroyWindow(mpWindow);
		glfwTerminate();
	}
}