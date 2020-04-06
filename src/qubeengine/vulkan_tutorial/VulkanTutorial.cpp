#include <qubeengine/vulkan_tutorial/VulkanTutorial.h>
#include <stdexcept>
#include <algorithm> //min and max functions
#include <cstdint> //Necessary for UINT32_MAX
#include <iostream>
#include <map>
#include <set>
#include <fstream>

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
		mValidationLayers(std::vector<const char*> { "VK_LAYER_KHRONOS_validation" }),
		mDeviceExtensions(std::vector<const char*> { VK_KHR_SWAPCHAIN_EXTENSION_NAME })
	{}

	void VulkanTutorial::run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	///Section 1 - Setup

	//Tutorial 2: Instance Creation
	void VulkanTutorial::cleanup()
	{
		cleanupSwapchain();

		vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

		vkDestroyBuffer(mDevice, mIndexBuffer, nullptr);
		vkFreeMemory(mDevice, mIndexBufferMemory, nullptr);

		vkDestroyBuffer(mDevice, mVertexBuffer, nullptr);
		vkFreeMemory(mDevice, mVertexBufferMemory, nullptr);

		for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

		vkDestroyDevice(mDevice, nullptr);

		if (ENABLE_VAL_LAYERS)
			destroyDebugUtilsMessengerEXT(mVulkanInstance, mDebugMessenger, nullptr);

		//Make sure the surface is destroyed before the instance.
		vkDestroySurfaceKHR(mVulkanInstance, mSurface, nullptr);
		vkDestroyInstance(mVulkanInstance, nullptr);

		glfwDestroyWindow(mpWindow);
		glfwTerminate();
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

		if (ENABLE_VAL_LAYERS && !validateRequiredInstanceExtensionSupport(requiredExtensions))
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
	void VulkanTutorial::initVulkan()
	{
		createInstance();					
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
	}
	void VulkanTutorial::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		mpWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Vulkan!", nullptr, nullptr);
		glfwSetWindowUserPointer(mpWindow, this);
		glfwSetFramebufferSizeCallback(mpWindow, framebufferResizeCallback);
	}
	void VulkanTutorial::mainLoop()
	{
		static int TICKS_PER_SECOND = 60;
		static float TIME_SLICE = 1.0f / (float)TICKS_PER_SECOND;
		static uint64 ticks = 0;
		static uint64 frames = 0;
		double lastTime = glfwGetTime();
		double currentTime = 0.0f;
		double elapsedTime = 0.0f;
		double lagTime = 0.0f;
		
		while (!glfwWindowShouldClose(mpWindow))
		{
			currentTime = glfwGetTime();
			elapsedTime = currentTime - lastTime;
			lastTime = currentTime;
			lagTime += elapsedTime;
		
			while (lagTime >= TIME_SLICE)
			{
				lagTime -= TIME_SLICE;
				ticks++;
				glfwPollEvents();
		
				if (ticks % TICKS_PER_SECOND == 0)
				{
					std::cout << "Ticks: " << std::to_string(TICKS_PER_SECOND) + " | FPS: " + std::to_string(frames) << std::endl;
					frames = 0;
				}
		
				if (ticks % 360 == 0)
				{
					ticks = 0;
				}
			}
		
			++frames;
			drawFrame();
		}
		
		vkDeviceWaitIdle(mDevice);
	}
	bool VulkanTutorial::validateRequiredInstanceExtensionSupport(const std::vector<const char*>& requiredExtensions)
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

	//Tutorial 3: Validation Layers
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
	void VulkanTutorial::setupDebugMessenger()
	{
		if (!ENABLE_VAL_LAYERS) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (createDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up debug messenger.");
		}
		else
		{
			std::cout << "Successfully created DebugUtilsMessengerEXT!" << std::endl;
		}
	}
	
	//Tutorial 4: Physical Devices and Queue Families
	QueueFamilyIndices VulkanTutorial::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		//Logic to find queue family indices to populate struct with

		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		uint32 i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}

			++i;
		}

		//std::cout << std::boolalpha << graphicsFamily.has_value() << std::endl;//false
		//graphicsFamily = 0;
		//std::cout << std::boolalpha << graphicsFamily.has_value() << std::endl;//true

		return indices;
	}
	void VulkanTutorial::pickPhysicalDevice()
	{
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan Support.");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, devices.data());

		std::multimap<uint32, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			uint32 score = rateDeviceSuitability(device);
			candidates.insert({ score, device });
		}

		if (candidates.rbegin()->first > 0)
		{
			mPhysicalDevice = candidates.rbegin()->second;
		}
		else
		{
			throw std::runtime_error("Failed to find a suitable GPU.");
		}

		std::cout << std::to_string(devices.size()) << " devices found!" << std::endl;
	}
	uint32 VulkanTutorial::rateDeviceSuitability(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		uint32 score = 0;

		//Discrete GPUs have a significant performance advantage.
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		//Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		//Example if app needed geometry shaders -> return 0 since
		//the application wouldn't function without geometry shaders
		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}

		//If we don't have the required queue families, (in our case just graphics)
		//then this gpu is not suitable for use
		if (!findQueueFamilies(device).isComplete())
		{
			return 0;
		}

		//This device doesn't have support for extensions that we want to uses
		if (!validateRequiredDeviceExtensionSupport(device))
		{
			return 0;
		}

		//Make sure this device supports at least one image format
		SwapChainSupportDetails swapChainSupport = querySwapchainSupport(device);
		if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
		{
			return 0;
		}


		return score;
	}
	
	//Tutorial 5: Logical Devices and Queues
	void VulkanTutorial::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32 queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		//Will add features we're gonna use here in the future.

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32>(mDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

		if (ENABLE_VAL_LAYERS)
		{
			//Ignored in newer versions of Vulkan, but good to have for backwards compatability
			createInfo.enabledLayerCount = static_cast<uint32>(mValidationLayers.size());
			createInfo.ppEnabledLayerNames = mValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create the logical device.");
		}
		else
		{
			std::cout << "Successfully created logical device!" << std::endl;
		}

		vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
		vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
	}

	///Section 2 - Presentation

	//Tutorial 6: Window Surface
	void VulkanTutorial::createSurface()
	{
		if (glfwCreateWindowSurface(mVulkanInstance, mpWindow, nullptr, &mSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface.");
		}
		else
		{
			std::cout << "Successfully created window surface!" << std::endl;
		}
	}
	
	//Tutorial 7: Swapchains
	VkExtent2D VulkanTutorial::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(mpWindow, &width, &height);

			VkExtent2D actualExtent = { width, height };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}
	VkPresentModeKHR VulkanTutorial::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		//If triple buffering is not supported, just use first in first out buffering.
		//Most similar to modern day vsync.
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkSurfaceFormatKHR VulkanTutorial::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		//If we don't find what we're looking for, just settle on the first format.
		return availableFormats[0];
	}
	void VulkanTutorial::createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = querySwapchainSupport(mPhysicalDevice);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;//Always 1 unless developing a stereoscopic 3D application
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
		uint32 queueFamilyIndices[]{ indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			//CONCURRENT - Images can be used across multiple queue families 
			//without explicit ownership transfers.
			//Concurrent mode requires at least 2 different queue families.
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			//EXCLUSIVE - An image is owned by one queue family at a time 
			//and ownership must be explicitly transfered before using it in 
			//another queue family. This option offers the best performance.
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		//We can specify that a certain transform should be applied to images in 
		//the swap chain if it is supported (supportedTransforms in capabilities), 
		//like a 90 degree clockwise rotation or horizontal flip. To specify that 
		//you do not want any transformation, simply specify the current t
		//ransformation.
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		//The compositeAlpha field specifies if the alpha channel should be used 
		//for blending with other windows in the window system. You'll almost 
		//always want to simply ignore the alpha channel, hence 
		//VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		//The presentMode member speaks for itself. If the clipped member is set 
		//to VK_TRUE then that means that we don't care about the color of pixels 
		//that are obscured, for example because another window is in front of 
		//them. Unless you really need to be able to read these pixels back and 
		//get predictable results, you'll get the best performance by enabling 
		//clipping.
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		//With Vulkan it's possible that your swap chain becomes invalid or 
		//unoptimized while your application is running, for example because 
		//the window was resized. In that case the swap chain actually needs to 
		//be recreated from scratch and a reference to the old one must be 
		//specified in this field. For now, we'll assume that we'll only ever
		//create one swap chain.
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swapchain.");
		}
		else
		{
			std::cout << "Successfully created swapchain!" << std::endl;
		}

		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr);
		mSwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data());
		
		mSwapchainImageFormat = surfaceFormat.format;
		mSwapchainExtent = extent;
	}
	SwapChainSupportDetails VulkanTutorial::querySwapchainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
	bool VulkanTutorial::validateRequiredDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
	
	//Tutorial 8: Image Views
	void VulkanTutorial::createImageViews()
	{
		mSwapchainImageViews.resize(mSwapchainImages.size());

		for (std::size_t i = 0; i < mSwapchainImages.size(); ++i)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = mSwapchainImages[i];
			//The viewType and format fields specify how the image data should 
			//be interpreted. The viewType parameter allows you to treat images 
			//as 1D textures, 2D textures, 3D textures and cube maps.
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = mSwapchainImageFormat;
			//The components field allows you to swizzle the color channels 
			//around. For example, you can map all of the channels to the red 
			//channel for a monochrome texture. You can also map constant 
			//values of 0 and 1 to a channel. In our case we'll stick to the 
			//default mapping.
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			//The subresourceRange field describes what the image's purpose is 
			//and which part of the image should be accessed. Our images will 
			//be used as color targets without any mipmapping levels or 
			//multiple layers.
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapchainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image view.");
			}
			else
			{
				std::cout << "Successfully created image view: " << std::to_string(i) << "!" << std::endl;
			}
		}
	}
	
	///Section 3 - Setup

	//Tutorial 9: Introduction
	void VulkanTutorial::createGraphicsPipeline()
	{
		std::vector<char> vertShaderCode = readFile("../../../../res/shaders/vert.spv");
		std::vector<char> fragShaderCode = readFile("../../../../res/shaders/frag.spv");
		std::cout << "Vertex Shader Size: " << std::to_string(vertShaderCode.size()) << std::endl;
		std::cout << "Fragment Shader Size: " << std::to_string(vertShaderCode.size()) << std::endl;

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

		vertShaderStageInfo.module = vertShaderModule;
		//Tell the compiler the function to invoke/the entrypoint for the shader
		vertShaderStageInfo.pName = "main";

		//Can be used to set constants in the shader if you want to change them at compile time
		vertShaderStageInfo.pSpecializationInfo = nullptr; //optional

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = nullptr; //optional

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		
		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions(); 

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mSwapchainExtent.width; //Not the same as window width/height
		viewport.height = (float)mSwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = mSwapchainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near 
		//and far planes are clamped to them as opposed to discarding them. This is useful 
		//in some special cases like shadow maps. Using this requires enabling a GPU feature.
		rasterizer.depthClampEnable = VK_FALSE;
		//If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through 
		//the rasterizer stage. This basically disables any output to the framebuffer.
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//The polygonMode determines how fragments are generated for geometry.
		//Using any other mode besides fill requires enabling a gpu feature.
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		//The linewidth member describes the thickness of lines in terms of
		//number of fragments. The max line width supported depends on the 
		//hardware. Any thicker than 1.0f requires the wideLines gpu feature
		//to be enabled
		rasterizer.lineWidth = 1.0f;
		//The cullMode variable determines the type of face culling to use. You can 
		//disable culling, cull the front faces, cull the back faces or both. 
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		//The frontFace variable specifies the vertex order for faces to be considered 
		//front-facing and can be clockwise or counterclockwise.
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		//Multisample requires enabling a gpu feature
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; //Optional
		multisampling.pSampleMask = nullptr; //Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; //Optional
		multisampling.alphaToOneEnable = VK_FALSE; //Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout.");
		}
		else
		{
			std::cout << "Successfully created pipeline layout!" << std::endl;
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = mPipelineLayout;
		pipelineInfo.renderPass = mRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline.");
		}
		else
		{
			std::cout << "Successfully create graphics pipeline!" << std::endl;
		}

		vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
		vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
	}

	//Tutorial 10: Shader Modules
	std::vector<char> VulkanTutorial::readFile(const std::string& fileName)
	{
		//ate: start reading at the end of the file
		//binary: read the file as binary
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file.");
		}
		else
		{
			std::cout << "Successfully opened file: " << fileName << std::endl;
		}

		//We can use the read position to determine the size of the file
		//and allocate a buffer.
		std::size_t fileSize = (std::size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		//Return to the beginning of the file to to all the bytes
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	VkShaderModule VulkanTutorial::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module.");
		}
		else
		{
			std::cout << "Successfully create shader module!" << std::endl;
		}

		return shaderModule;
	}

	//Tutorial 11: Render Passes
	void VulkanTutorial::createRenderPass()
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = mSwapchainImageFormat;
		//The format of the color attachment should match the format of the swap chain images, 
		//and we're not doing anything with multisampling yet, so we'll stick to 1 sample.
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//The loadOp and storeOp determine what to do with the data in the attachment before 
		//rendering and after rendering.
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear the values to a constant at the start
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //Rendered contents will be stored in memory and can be read later
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = 
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass.");
		}
		else
		{
			std::cout << "Successfully created render pass!" << std::endl;
		}
	}

	///Section 4 - Drawing

	//Tutorial 13: Framebuffers
	void VulkanTutorial::createFramebuffers()
	{
		mSwapchainFramebuffers.resize(mSwapchainImageViews.size());

		for (std::size_t i = 0; i < mSwapchainImageViews.size(); ++i)
		{
			VkImageView attachments[] =
			{
				mSwapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = mRenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = mSwapchainExtent.width;
			framebufferInfo.height = mSwapchainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapchainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer with id: " + std::to_string(i));
			}
			else
			{
				std::cout << "Successfully created framebuffer with id : " + std::to_string(i) << "!" << std::endl;
			}
		}
	}

	//Tutorial 14: Command Buffers
	void VulkanTutorial::createCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);
		
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = 0; // Optional

		if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create command pool.");
		}
		else
		{
			std::cout << "Successfully created command pool!" << std::endl;
		}
	}
	void VulkanTutorial::createCommandBuffers()
	{
		mCommandBuffers.resize(mSwapchainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCommandPool;

		//VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
		//VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

		if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate command buffers.");
		}
		else
		{
			std::cout << "Successfully allocated command buffers!" << std::endl;
		}

		for (std::size_t i = 0; i < mCommandBuffers.size(); ++i)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to begin recording command buffer with id:" + std::to_string(i));
			}
			else
			{
				std::cout << "Successfully recording command buffer with id: " << std::to_string(i) << "!" << std::endl;
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = mRenderPass;
			renderPassInfo.framebuffer = mSwapchainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = mSwapchainExtent;

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			//VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded in the primary command buffer 
				//itself and no secondary command buffers will be executed.
			//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands will be executed from 
				//secondary command buffers.
			vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

			VkBuffer vertexBuffers[] = { mVertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(mCommandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(mCommandBuffers[i], mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[i], 0, nullptr);
			vkCmdDrawIndexed(mCommandBuffers[i], static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
			vkCmdEndRenderPass(mCommandBuffers[i]);

			if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) 
			{
				throw std::runtime_error("Failed to record command buffer with id:" + std::to_string(i));
			}
			else
			{
				std::cout << "Successfully recorded command buffer with id: " << std::to_string(i) << "!" << std::endl;
			}
		}
	}

	//Tutorial 15: Rendering and Presentation

	//Acquire an image from the swap chain
	//Execute the command buffer with that image as attachment in the framebuffer
	//Return the image to the swap chain for presentation
	void VulkanTutorial::drawFrame()
	{
		vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
		//Fences are mainly designed to synchronize your application itself 
		//with rendering operation, whereas semaphores are used to synchronize 
		//operations within or across command queues.We want to synchronize the 
		//queue operations of draw commands and presentation, which makes 
		//semaphores the best fit.

		uint32 mImageIndex;
		//Using the maximum value of a 64 bit unsigned integer disables the timeout.
		VkResult result = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &mImageIndex);
	
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swapchain image.");
		}

		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (mImagesInFlight[mImageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(mDevice, 1, &mImagesInFlight[mImageIndex], VK_TRUE, UINT64_MAX);
		}

		// Mark the image as now being in use by this frame
		mImagesInFlight[mImageIndex] = mInFlightFences[mCurrentFrame];

		updateUniformBuffer(mImageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffers[mImageIndex];

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);
		if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to submit draw command buffer.");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { mSwapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &mImageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
		{
			//It is important to do this after vkQueuePresentKHR to ensure that the semaphores are in a consistent
			//state, otherwise a signalled semaphore may never be properly waited upon.
			mFramebufferResized = false;
			recreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image.");
		}

		mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanTutorial::createSyncObjects()
	{
		mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		mImagesInFlight.resize(mSwapchainImages.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

				throw std::runtime_error("Failed to create synchronization objects for a frame.");
			}
			else
			{
				std::cout << "Successfully created synchronization objects for a frame!" << std::endl;
			}
		}
	}

	//Tutorial 16: Swapchain Recreation
	void VulkanTutorial::recreateSwapchain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(mpWindow, &width, &height);

		while (width == 0 || height == 0) 
		{
			glfwGetFramebufferSize(mpWindow, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(mDevice);

		cleanupSwapchain();

		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
	}
	void VulkanTutorial::cleanupSwapchain()
	{
		for (VkFramebuffer framebuffer : mSwapchainFramebuffers)
		{
			vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
		}

		vkFreeCommandBuffers(mDevice, mCommandPool, 
			static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

		vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
		vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

		for (auto imageView : mSwapchainImageViews)
		{
			vkDestroyImageView(mDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

		for (size_t i = 0; i < mSwapchainImages.size(); i++) 
		{
			vkDestroyBuffer(mDevice, mUniformBuffers[i], nullptr);
			vkFreeMemory(mDevice, mUniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
	}
	void VulkanTutorial::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<VulkanTutorial*>(glfwGetWindowUserPointer(window));
		app->mFramebufferResized = true;
	}

	///Section 5 - Vertex Buffers

	//Tutorial 18: Vertex Buffer Creation
	void VulkanTutorial::createVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, mVertices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

		copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}
	uint32_t VulkanTutorial::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
			{
				return i;
			}
		}

		throw std::runtime_error("Failed to find suitable memory type.");
	}

	//Tutorial 19: Staging Buffer
	void VulkanTutorial::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
		VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
	}
	void VulkanTutorial::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mGraphicsQueue);

		vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
	}
	
	//Tutorial 20: Index Buffer
	void VulkanTutorial::createIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, mIndices.data(), (size_t)bufferSize);
		vkUnmapMemory(mDevice, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

		copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
	}

	///Section 6 - Uniform Buffers

	//Tutorial 21: Descriptor Layout and Buffer
	void VulkanTutorial::createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create descriptor set layout.");
		}
	}
	void VulkanTutorial::createUniformBuffers() 
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		mUniformBuffers.resize(mSwapchainImages.size());
		mUniformBuffersMemory.resize(mSwapchainImages.size());

		for (size_t i = 0; i < mSwapchainImages.size(); i++) 
		{
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);
		}
	}
	void VulkanTutorial::updateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(69.0f), mSwapchainExtent.width / (float)mSwapchainExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		void* data;
		vkMapMemory(mDevice, mUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(mDevice, mUniformBuffersMemory[currentImage]);
	}

	//Tutorial 22: Descriptor Pool and Sets
	void VulkanTutorial::createDescriptorPool()
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = static_cast<uint32_t>(mSwapchainImages.size());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(mSwapchainImages.size());

		if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create descriptor pool.");
		}
	}

	void VulkanTutorial::createDescriptorSets() 
	{
		std::vector<VkDescriptorSetLayout> layouts(mSwapchainImages.size(), mDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(mSwapchainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(mSwapchainImages.size());
		if (vkAllocateDescriptorSets(mDevice, &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate descriptor sets.");
		}

		for (size_t i = 0; i < mSwapchainImages.size(); i++) 
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = mUniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(mDevice, 1, &descriptorWrite, 0, nullptr);
		}
	}
}