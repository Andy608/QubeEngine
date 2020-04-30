#ifndef QUBEENGINE_RENDER_VULKANRENDERER_H_ 
#define QUBEENGINE_RENDER_VULKANRENDERER_H_

#include <qubeengine/core/Common.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <chrono>
#include <vector>
#include <optional>
#include <array>
#include <string>

namespace qe
{
	struct UniformBufferObject 
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		//A vertex binding describes at which rate to load data from memory throughout the vertices.
		//It specifies the number of bytes between data entriesand whether to move to the next data entry after 
		//each vertex or after each instance.
		inline static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		//An attribute description struct describes how to extract a vertex attribute from a chunk of vertex data 
		//originating from a binding description. We have two attributes, position and color, so we need two attribute 
		//description structs.
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const 
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

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
		const std::vector<const char*> mDeviceExtensions;
		const std::vector<const char*> mValidationLayers;
		static const int MAX_FRAMES_IN_FLIGHT = 2;

		GLFWwindow* mpWindow = nullptr;

		VkInstance mVulkanInstance = nullptr;
		VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
		VkSurfaceKHR mSurface = nullptr;
		
		VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
		VkDevice mDevice = nullptr;
		
		VkQueue mGraphicsQueue = nullptr;
		VkQueue mPresentQueue = nullptr;
		
		VkSwapchainKHR mSwapchain = nullptr;
		std::vector<VkImage> mSwapchainImages;
		VkFormat mSwapchainImageFormat;
		VkExtent2D mSwapchainExtent;
		std::vector<VkImageView> mSwapchainImageViews;
		std::vector<VkFramebuffer> mSwapchainFramebuffers;
		
		VkRenderPass mRenderPass;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mGraphicsPipeline;

		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;

		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;
		std::vector<VkFence> mImagesInFlight;
		std::size_t mCurrentFrame = 0;
		
		bool mFramebufferResized = false;
		std::string modelPath;
		std::string texturePath;
		const std::string RES_FILE = "../../../../res/resource_locations.txt";

		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;
		VkBuffer mVertexBuffer;
		VkDeviceMemory mVertexBufferMemory;

		VkBuffer mIndexBuffer;
		VkDeviceMemory mIndexBufferMemory;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;

		VkDescriptorPool mDescriptorPool;
		std::vector<VkDescriptorSet> mDescriptorSets;

		VkImage mTextureImage;
		VkDeviceMemory mTextureImageMemory;
		VkImageView mTextureImageView;
		VkSampler mTextureSampler;

		VkImage mDepthImage;
		VkDeviceMemory mDepthImageMemory;
		VkImageView mDepthImageView;

		glm::vec3 mCameraPosition = glm::vec3(3.0f, 3.0f, 2.5f);

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

		//Tutorial 16: Swapchain Recreation
		void recreateSwapchain();
		void cleanupSwapchain();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		///Section 5 - Vertex Buffers

		//Tutorial 18: Vertex Buffer Creation
		void createVertexBuffer();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		//Tutorial 19: Staging Buffer
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		
		//Tutorial 20: Index Buffer
		void createIndexBuffer();

		///Section 6 - Uniform Buffers

		//Tutorial 21: Descriptor Layout and Buffer
		void createDescriptorSetLayout();
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);

		//Tutorial 22: Descriptor Pool and Sets
		void createDescriptorPool();
		void createDescriptorSets();

		///Section 7 - Texture Mapping

		//Tutorial 23: Images
		void createTextureImage();
		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	
		//Tutorial 24: Image View and Sampler
		void createTextureImageView();
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		void createTextureSampler();

		///Section 8 - Depth Buffering
		void createDepthResources();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);

		///Section 9 - Loading Models
		void loadModel();
		void initResPaths();

		void processInput(GLFWwindow* window, float deltaTime);
	};
}

namespace std
{
	//We need to create a hash function for our custom Vertex struct so we can use it in maps.
	template<> struct hash<qe::Vertex> 
	{
		size_t operator()(qe::Vertex const& vertex) const 
		{
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

#endif