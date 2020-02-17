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

		void run();

	private:
		GLFWwindow* mpWindow;
		VkInstance mVulkanInstance;

		void initWindow();
		
		void initVulkan();
		void createInstance();
		bool validateRequiredExtensions(const std::vector<VkExtensionProperties>& availableExtensions, const char** requiredExtensions, uint32 requiredExtensionCount);

		void mainLoop();
		void cleanup();
	};
}

#endif