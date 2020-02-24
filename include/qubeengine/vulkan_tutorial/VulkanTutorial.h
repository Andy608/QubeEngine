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

		bool isComplete()
		{
			return graphicsFamily.has_value();
		}
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
		VkDebugUtilsMessengerEXT mDebugMessenger;
		const std::vector<const char*> mValidationLayers;

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

		void initWindow();
		void initVulkan();
		void createInstance();
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool validateRequiredExtensions(const std::vector<const char*>& requiredExtensions);
		bool checkValidationLayerSupport();
		void pickPhysicalDevice();
		uint32 rateDeviceSuitability(VkPhysicalDevice device);
		//bool isDeviceSuitable(VkPhysicalDevice device);
		
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

		std::vector<const char*> getRequiredExtensions();

		void mainLoop();
		void cleanup();
	};
}

#endif