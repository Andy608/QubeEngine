#include <qubeengine/vulkan_tutorial/VulkanTutorial.h>
#include <stdexcept>
#include <iostream>

namespace qe
{
	const int VulkanTutorial::WINDOW_WIDTH = 800;
	const int VulkanTutorial::WINDOW_HEIGHT = 600;

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
	}

	void VulkanTutorial::createInstance()
	{
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

		uint32 glfwExtensionCount = 0;
		const char** glfwRequiredExtensions;
		glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwRequiredExtensions;
		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &mVulkanInstance);

		if (vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vulkan instance.");
		}
		else
		{
			std::cout << "Successfully initialized Vulkan!" << std::endl;
		}

		uint32 extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::cout << "\nRequired glfw extensions:" << std::endl;
		for (auto i = 0; i < glfwExtensionCount; ++i)
		{
			std::cout << "\t" << glfwRequiredExtensions[i] << std::endl;
		}

		std::cout << "\nAvailable extensions:" << std::endl;
		for (const auto& extension : availableExtensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		if (!validateRequiredExtensions(availableExtensions, glfwRequiredExtensions, glfwExtensionCount))
		{
			throw std::runtime_error("Failed to load requried glfw extensions.");
		}
		else
		{
			std::cout << "Successfully loaded required glfw extensions!" << std::endl;
		}
	}

	bool VulkanTutorial::validateRequiredExtensions(const std::vector<VkExtensionProperties>& availableExtensions, const char** requiredExtensions, uint32 requiredExtensionCount)
	{
		bool success = true;
		for (auto i = 0; i < requiredExtensionCount; ++i)
		{
			bool isFound = false;
			for (const auto& loadedExtension : availableExtensions)
			{
				if (strcmp(requiredExtensions[i], loadedExtension.extensionName))
				{
					isFound = true;
				}
			}

			if (!isFound)
			{
				success = false;
				break;
			}
		}

		return success;
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
		vkDestroyInstance(mVulkanInstance, nullptr);
		glfwDestroyWindow(mpWindow);
		glfwTerminate();
	}
}