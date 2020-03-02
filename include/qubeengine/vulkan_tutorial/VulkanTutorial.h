#ifndef QUBEENGINE_RENDER_VULKANRENDERER_H_ 
#define QUBEENGINE_RENDER_VULKANRENDERER_H_

#include <qubeengine/core/Common.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>
#include <string>

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
		GLFWwindow* mpWindow = nullptr;
		VkInstance mVulkanInstance = nullptr;
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = nullptr;
		VkSurfaceKHR mSurface = nullptr;
		VkQueue mGraphicsQueue = nullptr;
		VkQueue mPresentQueue = nullptr;
		VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
		const std::vector<const char*> mDeviceExtensions;
		const std::vector<const char*> mValidationLayers;
		VkSwapchainKHR mSwapchain = nullptr;
		std::vector<VkImage> mSwapchainImages = {};
		VkFormat mSwapchainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D mSwapchainExtent = {};
		std::vector<VkImageView> mSwapchainImageViews;
		VkRenderPass mRenderPass;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mGraphicsPipeline;
		std::vector<VkFramebuffer> mSwapchainFramebuffers;
		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;
		//VkSemaphore mImageAvailableSemaphore;
		//VkSemaphore mRenderFinishedSemaphore;
		const int mMAX_FRAMES_IN_FLIGHT = 2;
		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;
		std::vector<VkFence> mImagesInFlight;
		std::size_t mCurrentFrame = 0;


		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		///https://vulkan-tutorial.com/
		///Section 1 - Setup

		//Tutorial 2: Instance Creation
		void cleanup();
		void createInstance();
		void initVulkan();
		void initWindow();
		void mainLoop();
		bool validateRequiredInstanceExtensionSupport(const std::vector<const char*>& requiredExtensions);

		//Tutorial 3: Validation Layers
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void setupDebugMessenger();
		
		//Tutorial 4: Physical Devices and Queue Families
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void pickPhysicalDevice();
		uint32 rateDeviceSuitability(VkPhysicalDevice device);

		//Tutorial 5: Logical Devices and Queues
		void createLogicalDevice();

		///Section 2 - Presentation

		//Tutorial 6: Window Surface
		void createSurface();

		//Tutorial 7: Swapchains
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		void createSwapChain();
		SwapChainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
		bool validateRequiredDeviceExtensionSupport(VkPhysicalDevice device);

		//Tutorial 8: Image Views
		void createImageViews();

		///Section 3 - Setup
		
		//Tutorial 9: Introduction
		void createGraphicsPipeline();

		//Tutorial 10: Shader Modules
		static std::vector<char> readFile(const std::string& fileName);
		VkShaderModule createShaderModule(const std::vector<char>& code);

		//Tutorial 11: Render Passes
		void createRenderPass();

		///Section 4 - Drawing

		//Tutorial 13: Framebuffers
		void createFramebuffers();

		//Tutorial 14: Command Buffers
		void createCommandPool();
		void createCommandBuffers();

		//Tutorial 15: Rendering and Presentation
		void drawFrame();
		void createSyncObjects();
	};
}

#endif