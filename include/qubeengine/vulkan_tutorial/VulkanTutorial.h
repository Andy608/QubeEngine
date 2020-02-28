#ifndef QUBEENGINE_RENDER_VULKANRENDERER_H_ 
#define QUBEENGINE_RENDER_VULKANRENDERER_H_

#include <qubeengine/core/Common.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

namespace qe
{
	struct QueueFamilyIndices
	{
		std::optional<uint32> graphicsFamily;
		std::optional<uint32> presentFamily;

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

	class VulkanTutorial
	{
	public:
		static const int WINDOW_WIDTH;
		static const int WINDOW_HEIGHT;
		static const bool ENABLE_VAL_LAYERS;

		static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);

		static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		VulkanTutorial();

		void run();

	private:
		GLFWwindow* mpWindow;
		VkInstance mVulkanInstance;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice;
		VkSurfaceKHR mSurface;
		VkQueue mGraphicsQueue;
		VkQueue mPresentQueue;
		VkDebugUtilsMessengerEXT mDebugMessenger;
		const std::vector<const char*> mDeviceExtensions;
		const std::vector<const char*> mValidationLayers;
		VkSwapchainKHR mSwapchain;
		std::vector<VkImage> mSwapchainImages;
		VkFormat mSwapchainImageFormat;
		VkExtent2D mSwapchainExtent;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		//Tutorial 1: Instance Creation
		void initWindow();
		void initVulkan();
		void createInstance();
		bool validateRequiredInstanceExtensionSupport(const std::vector<const char*>& requiredExtensions);
		void mainLoop();
		void cleanup();

		//Tutorial 2: Validation Layers
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
		
		//Tutorial 3: Physical Devices
		void pickPhysicalDevice();
		uint32 rateDeviceSuitability(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		//Tutorial 4: Locail Devices
		void createLogicalDevice();

		//Tutorial 5: Window Surface
		void createSurface();

		//Tutorial 6: Swapchains
		bool validateRequiredDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createSwapChain();
	};
}

#endif