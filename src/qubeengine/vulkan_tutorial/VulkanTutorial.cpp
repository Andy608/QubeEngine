#include <qubeengine/vulkan_tutorial/VulkanTutorial.h>
#include <stdexcept>
#include <algorithm> //min and max functions
#include <cstdint> //Necessary for UINT32_MAX
#include <iostream>
#include <map>
#include <set>
#include <string>

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
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
	}

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

	//bool VulkanTutorial::isDeviceSuitable(VkPhysicalDevice device)
	//{
	//	VkPhysicalDeviceProperties deviceProperties;
	//	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//
	//	VkPhysicalDeviceFeatures deviceFeatures;
	//	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	//
	//	return true;
	//}

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

	VkExtent2D VulkanTutorial::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { WINDOW_WIDTH, WINDOW_HEIGHT };
			//actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			//actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			
			return actualExtent;
		}
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

	void VulkanTutorial::mainLoop()
	{
		while (!glfwWindowShouldClose(mpWindow))
		{
			glfwPollEvents();
		}
	}

	void VulkanTutorial::cleanup()
	{
		for (auto imageView : mSwapchainImageViews)
		{
			vkDestroyImageView(mDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
		vkDestroyDevice(mDevice, nullptr);

		if (ENABLE_VAL_LAYERS)
			destroyDebugUtilsMessengerEXT(mVulkanInstance, mDebugMessenger, nullptr);

		//Make sure the surface is destroyed before the instance.
		vkDestroySurfaceKHR(mVulkanInstance, mSurface, nullptr);
		vkDestroyInstance(mVulkanInstance, nullptr);

		glfwDestroyWindow(mpWindow);
		glfwTerminate();
	}
}