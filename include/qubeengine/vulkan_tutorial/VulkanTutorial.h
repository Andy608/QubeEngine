#ifndef QUBEENGINE_RENDER_VULKANRENDERER_H_ 
#define QUBEENGINE_RENDER_VULKANRENDERER_H_

#include <qubeengine/core/Common.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace qe
{
	class VulkanTutorial
	{
	public:
		static const int WINDOW_WIDTH;
		static const int WINDOW_HEIGHT;
		static const bool ENABLE_VAL_LAYERS;

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
		bool validateRequiredExtensions(const std::vector<const char*>& requiredExtensions);
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();

		void mainLoop();
		void cleanup();
	};
}

#endif