#ifndef __H_ENGINE__
#define __H_ENGINE__

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
#include <utility>
#include <string>

class Window
{
	public:
		Window(VkInstance instance, int width, int height, const char *title);
		~Window();

		bool shouldClose() const;

		const vk::SurfaceKHR & getSurface() const;

		vk::Extent2D getWindowSize() const;

		GLFWwindow * getWindow() const;

	private:
		GLFWwindow * m_window;
		vk::SurfaceKHR m_surface;
};

struct QueueFamilyIndices
{
	uint32_t graphicsFamily = -1;
	uint32_t transferFamily = -1;

	bool isComplete() const
	{
		return graphicsFamily >= 0 && transferFamily >= 0;
	}
};

class Engine
{
	private:
		static Engine *m_engine;
	public:
		static bool isDebug;
		static bool separateQueues;

	public:
		static PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
		static PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
		static PFN_vkDebugReportMessageEXT DebugReportMessage;

	private:
		static const std::string debugLayer;
		static const std::string appName;

	private:
		static VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
			uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix,
			const char* pMessage, void* pUserData);

	private:
		vk::UniqueInstance m_instance;
		vk::UniqueDebugReportCallbackEXT m_callback;
		std::unique_ptr<Window> m_window;
		vk::PhysicalDevice m_physicalDevice;
		vk::UniqueDevice m_device;

		QueueFamilyIndices m_familyIndices;
		vk::Queue m_graphicsQueue;
		vk::Queue m_transferQueue;
		vk::UniqueCommandPool m_gCommandPool;
		vk::UniqueCommandPool m_tCommandPool;

	private:
		Engine();
		Engine(Engine const &e) = delete;
		void operator=(Engine const &e) = delete;
		
		QueueFamilyIndices Engine::findQueueFamilies(const vk::PhysicalDevice& device) const;
		bool checkVKDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
		bool checkSwapchainSupport(const vk::PhysicalDevice& device) const;

		bool isDeviceSuitable(const vk::PhysicalDevice& device) const;

		void singleQueueOperation(const std::function<void(const vk::CommandBuffer&)> &lambda) const;

	public:
		static std::vector<const char*> vkGetRequiredValidationLayers();
		static std::vector<const char*> vkGetRequiredInstanceExtensions();
		static std::vector<const char*> vkGetRequiredDeviceExtensions();

	public:
		static Engine& getInstance()
		{
			if(m_engine == nullptr)
				m_engine = new Engine();

			return *m_engine;
		}

		static void resetInstance()
		{
			delete m_engine;
			m_engine = nullptr;
		}

		void createWindow(int width, int heigth);
		void selectPhysicalDevice();
		void createLogicalDevice();

		const vk::PhysicalDevice &getPhysicalDevice() const;
		const vk::Device &getDevice() const;
		Window * getWindow() const;


		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
		void createBuffer(vk::UniqueDeviceMemory &memory, vk::UniqueBuffer &buffer, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const;
		void copyBuffer(vk::Buffer dstBuffer, vk::Buffer srcBuffer, vk::DeviceSize size) const;
		void copyMemory(vk::DeviceMemory bufferMemory, vk::DeviceSize size, void* dataPointer, vk::DeviceSize offset = 0) const;
		void createBufferData(vk::UniqueDeviceMemory &memory, vk::UniqueBuffer &buffer, vk::DeviceSize size, vk::BufferUsageFlagBits usage, void* data) const;
		void createImage(vk::UniqueDeviceMemory& imageMemory, vk::UniqueImage& image, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
		void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange range) const;
		vk::Queue getPresentQueue() const;
		vk::CommandPool getWorkCommandPool() const;

		bool shouldClose() const;
		void debugMessage(const std::string &text) const;
};

#endif