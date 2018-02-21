#include "engine.hpp"

#include <iostream>
#include <set>
#include <vector>

Window::Window(VkInstance instance, int width, int height, const char *title)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	VkResult res = glfwCreateWindowSurface(instance, m_window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_surface));
	
	if (static_cast<vk::Result>(res) != vk::Result::eSuccess)
		throw std::runtime_error("Can't create VkSurface for presentation!");
}
	
Window::~Window()
{
	glfwDestroyWindow(m_window);
}

const vk::SurfaceKHR & Window::getSurface() const
{
	return m_surface;
}

vk::Extent2D Window::getWindowSize() const
{
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);

	return { uint32_t(width), uint32_t(height) };
}

GLFWwindow * Window::getWindow() const
{
	return m_window;
}
	
bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_window);
}

Engine *Engine::m_engine = nullptr;
bool Engine::isDebug = true;
bool Engine::separateQueues = false; //do not set to true - breaks debuggers
const std::string Engine::debugLayer{ "VK_LAYER_LUNARG_standard_validation" };
const std::string Engine::appName{ "Vulkan GL" };

PFN_vkCreateDebugReportCallbackEXT Engine::CreateDebugReportCallback;
PFN_vkDestroyDebugReportCallbackEXT Engine::DestroyDebugReportCallback;
PFN_vkDebugReportMessageEXT Engine::DebugReportMessage;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance                                  instance,
	const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDebugReportCallbackEXT*                   pCallback)
{
	return Engine::CreateDebugReportCallback(instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
	VkInstance                                  instance,
	VkDebugReportCallbackEXT                    callback,
	const VkAllocationCallbacks*                pAllocator)
{
	return Engine::DestroyDebugReportCallback(instance, callback, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
	VkInstance                                  instance,
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char*                                 pLayerPrefix,
	const char*                                 pMessage)
{
	return Engine::DebugReportMessage(instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}

Engine::Engine()
{
	vk::ApplicationInfo ai{ Engine::appName.c_str(), VK_MAKE_VERSION(1, 0, 0), Engine::appName.c_str(), VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0 };

	const auto requiredLayers = vkGetRequiredValidationLayers();
	const auto requiredExtensions = vkGetRequiredInstanceExtensions();

	vk::InstanceCreateInfo ici{ {}, &ai, uint32_t(requiredLayers.size()), requiredLayers.data(), uint32_t(requiredExtensions.size()), requiredExtensions.data() };

	m_instance = vk::createInstanceUnique(ici);

	if (Engine::isDebug)
	{
		Engine::CreateDebugReportCallback = PFN_vkCreateDebugReportCallbackEXT(m_instance->getProcAddr("vkCreateDebugReportCallbackEXT"));
		Engine::DestroyDebugReportCallback = PFN_vkDestroyDebugReportCallbackEXT(m_instance->getProcAddr("vkDestroyDebugReportCallbackEXT"));
		Engine::DebugReportMessage = PFN_vkDebugReportMessageEXT(m_instance->getProcAddr("vkDebugReportMessageEXT"));
		//Log every message or just warnings and errors
		//vk::DebugReportCallbackCreateInfoEXT dbgci{ ~vk::DebugReportFlagBitsEXT(), Engine::debugCallback };
		vk::DebugReportCallbackCreateInfoEXT dbgci{ vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError, Engine::debugCallback };
		m_callback = m_instance->createDebugReportCallbackEXTUnique(dbgci);
	}
}

VkBool32 VKAPI_CALL Engine::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
	uint64_t object, size_t location, int32_t messageCode,
	const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	std::cerr << "Vulkan: " << vk::to_string(static_cast<vk::DebugReportObjectTypeEXT>(objectType)) << ": " << pMessage << std::endl;
	return VK_FALSE;
}

std::vector<const char*> Engine::vkGetRequiredValidationLayers()
{
	std::vector<const char*> layers;

	if (Engine::isDebug)
		layers.push_back(Engine::debugLayer.c_str());

	return layers;
}

std::vector<const char*> Engine::vkGetRequiredInstanceExtensions()
{
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; ++i)
		extensions.push_back(glfwExtensions[i]);

	if (Engine::isDebug)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	return extensions;
}

std::vector<const char*> Engine::vkGetRequiredDeviceExtensions()
{
	std::vector<const char*> extensions;

	extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	extensions.push_back(VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME);

	return extensions;
}

void Engine::debugMessage(const std::string &text) const
{
	//m_instance->debugReportMessageEXT(vk::DebugReportFlagBitsEXT::eDebug, vk::DebugReportObjectTypeEXT::eDebugReportCallbackExt, 0, 0, 0, text, text);
}

void Engine::createWindow(int width, int height)
{
	m_window = std::make_unique<Window>(m_instance.get(), width, height, Engine::appName.c_str());
}

bool Engine::shouldClose() const
{
	return m_window->shouldClose();
}

bool Engine::checkVKDeviceExtensionSupport(const vk::PhysicalDevice& device) const
{
	std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

	std::set<std::string> requiredDevExtensions;

	const auto list = Engine::vkGetRequiredDeviceExtensions();
	requiredDevExtensions.insert(list.begin(), list.end());

	for (const auto &extProp : availableExtensions)
		requiredDevExtensions.erase(extProp.extensionName);

	return requiredDevExtensions.empty();
}

bool Engine::checkSwapchainSupport(const vk::PhysicalDevice& device) const
{
	const std::vector<vk::SurfaceFormatKHR> formats = device.getSurfaceFormatsKHR(m_window->getSurface());
	const std::vector<vk::PresentModeKHR> presentModes = device.getSurfacePresentModesKHR(m_window->getSurface());

	return !formats.empty() && !presentModes.empty();
}

QueueFamilyIndices Engine::findQueueFamilies(const vk::PhysicalDevice& device) const
{
	const std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device.getQueueFamilyProperties();

	QueueFamilyIndices indices;

	//std::cout << "Device queue families:" << std::endl;
	for (uint32_t i = 0; i < uint32_t(queueFamilyProperties.size()); ++i)
	{
		//std::cout << i << ". (" << queueFamilyProperties[i].queueCount << "): " << to_string(queueFamilyProperties[i].queueFlags) << std::endl;
		if (queueFamilyProperties[i].queueCount > 0)
		{
			bool supportGraphics = bool(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics);
			bool supportTransfer = bool(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer);
			bool supportSurface = device.getSurfaceSupportKHR(i, m_window->getSurface());

			if (!Engine::separateQueues)
			{
				if (supportGraphics && supportSurface && supportTransfer)
				{
					indices.graphicsFamily = indices.transferFamily = i;
					break;
				}
			}
			else
			{
				if (supportGraphics && supportSurface)
				{
					indices.graphicsFamily = i;
				}
				else if (supportTransfer)
				{
					indices.transferFamily = i;
				}

				if (indices.isComplete())
					break;
			}
		}
	}

	indices.transferFamily = indices.graphicsFamily;

	return indices;
}

uint32_t Engine::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const
{
	vk::PhysicalDeviceMemoryProperties memProperties = m_physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Engine::singleQueueOperation(const std::function<void(const vk::CommandBuffer&)> &lambda) const
{
	vk::CommandBufferAllocateInfo allocInfo{ m_tCommandPool.get(), vk::CommandBufferLevel::ePrimary, 1 };

	std::vector<vk::UniqueCommandBuffer> commandBuffers = m_device->allocateCommandBuffersUnique(allocInfo);

	vk::CommandBufferBeginInfo beginInfo{};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffers[0]->begin(beginInfo);
	
	lambda(commandBuffers[0].get());

	commandBuffers[0]->end();
	vk::SubmitInfo submitInfo{};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[0].get();

	m_transferQueue.submit(1, &submitInfo, vk::Fence());
	m_transferQueue.waitIdle();
}

void Engine::createBuffer(vk::UniqueDeviceMemory &bufferMemory, vk::UniqueBuffer &buffer, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const
{
	vk::BufferCreateInfo bufferInfo{ {}, size, usage, vk::SharingMode::eExclusive };
	
	buffer = m_device->createBufferUnique(bufferInfo);

	vk::MemoryRequirements memRequirements = m_device->getBufferMemoryRequirements(buffer.get());

	vk::MemoryAllocateInfo allocInfo{ memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties) };

	bufferMemory = m_device->allocateMemoryUnique(allocInfo);

	m_device->bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
}

void Engine::copyBuffer(const vk::Buffer dstBuffer, const vk::Buffer srcBuffer, const vk::DeviceSize size) const
{
	vk::BufferCopy copyRegion{ 0, 0, size };

	singleQueueOperation(
		[&](const vk::CommandBuffer &cmd)
		{
			cmd.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
		}
	);
}

void Engine::copyMemory(vk::DeviceMemory bufferMemory, vk::DeviceSize size, void* dataPointer, vk::DeviceSize offset) const
{
	void* data = m_device->mapMemory(bufferMemory, offset, size);
	memcpy(data, dataPointer, size);
	m_device->unmapMemory(bufferMemory);
}

void Engine::createBufferData(vk::UniqueDeviceMemory &memory, vk::UniqueBuffer &buffer, vk::DeviceSize size, vk::BufferUsageFlagBits usage, void* dataPointer) const
{
	vk::UniqueDeviceMemory stagingBufferMemory;
	vk::UniqueBuffer stagingBuffer;
	createBuffer(stagingBufferMemory, stagingBuffer, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	
	copyMemory(stagingBufferMemory.get(), size, dataPointer);

	createBuffer(memory, buffer, size, usage | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(buffer.get(), stagingBuffer.get(), size);
}

void Engine::createImage(vk::UniqueDeviceMemory& imageMemory, vk::UniqueImage& image, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	vk::ImageCreateInfo imci{ {}, vk::ImageType::e2D, format, { width, height, 1 }, 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive, 1, nullptr, vk::ImageLayout::eUndefined };
	image = m_device->createImageUnique(imci);

	vk::MemoryRequirements memRequirements = m_device->getImageMemoryRequirements(image.get());

	vk::MemoryAllocateInfo memInfo{ memRequirements.size, findMemoryType(memRequirements.memoryTypeBits, properties) };

	imageMemory = m_device->allocateMemoryUnique(memInfo);

	m_device->bindImageMemory(image.get(), imageMemory.get(), 0);
}

void Engine::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageSubresourceRange range) const
{
	singleQueueOperation(
		[&](const vk::CommandBuffer &cmd)
		{
			vk::PipelineStageFlags srcStageMask;
			vk::PipelineStageFlags dstStageMask;
			vk::AccessFlags srcAccessMask;
			vk::AccessFlags dstAccessMask;

			if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			{
				srcAccessMask = vk::AccessFlagBits();
				dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

				srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
				dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
			}

			vk::ImageMemoryBarrier barrier{ srcAccessMask, dstAccessMask, oldLayout, newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image, range };

			cmd.pipelineBarrier(
				srcStageMask, dstStageMask, 
				vk::DependencyFlags(), 
				{},
				{},
				{ barrier }
			);
		}
	);
}

bool Engine::isDeviceSuitable(const vk::PhysicalDevice& device) const
{
	const vk::PhysicalDeviceProperties prop = device.getProperties();
	const vk::PhysicalDeviceFeatures features = device.getFeatures();

	std::cout << prop.deviceName << std::endl;

	if (prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && features.tessellationShader && features.multiDrawIndirect)
	{
		bool extensionsPresent = checkVKDeviceExtensionSupport(device);
		bool swapChainAdequate = checkSwapchainSupport(device);

		QueueFamilyIndices indices = findQueueFamilies(device);

		return extensionsPresent && swapChainAdequate && indices.isComplete();
	}

	return false;
}

void Engine::selectPhysicalDevice()
{
	const std::vector<vk::PhysicalDevice> devices = m_instance->enumeratePhysicalDevices();

	std::cout << "Physical devices:" << std::endl;
	for (const vk::PhysicalDevice& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			m_familyIndices = findQueueFamilies(m_physicalDevice);
			return;
		}
	}

	throw std::runtime_error("No suitable physical devices!");
}

void Engine::createLogicalDevice()
{
	std::vector<vk::DeviceQueueCreateInfo> queueInfos;

	const float defaultPriority[] = { 1.0f, 1.0f };

	if (Engine::separateQueues)
	{
		vk::DeviceQueueCreateInfo graphicsQueue{ {}, m_familyIndices.graphicsFamily, 1, &defaultPriority[0] };
		vk::DeviceQueueCreateInfo transferQueue{ {}, m_familyIndices.transferFamily, 1, &defaultPriority[1] };

		queueInfos.push_back(graphicsQueue);
		queueInfos.push_back(transferQueue);
	}
	else
	{
		vk::DeviceQueueCreateInfo graphicsQueue{ {}, m_familyIndices.graphicsFamily, 2, defaultPriority };

		queueInfos.push_back(graphicsQueue);
	}

	const auto requiredExtensions = vkGetRequiredDeviceExtensions();

	vk::PhysicalDeviceFeatures features{};
	features.tessellationShader = VK_TRUE;
	features.multiDrawIndirect = VK_TRUE;

	vk::DeviceCreateInfo di{ {}, uint32_t(queueInfos.size()), queueInfos.data(), 0, nullptr, uint32_t(requiredExtensions.size()), requiredExtensions.data(), &features };

	m_device = m_physicalDevice.createDeviceUnique(di);

	if (Engine::separateQueues)
	{
		m_graphicsQueue = m_device->getQueue(m_familyIndices.graphicsFamily, 0);
		m_transferQueue = m_device->getQueue(m_familyIndices.transferFamily, 0);
	}
	else
	{
		m_graphicsQueue = m_device->getQueue(m_familyIndices.graphicsFamily, 0);
		m_transferQueue = m_device->getQueue(m_familyIndices.graphicsFamily, 1);
	}

	vk::CommandPoolCreateInfo gci{ ~vk::CommandPoolCreateFlagBits(), m_familyIndices.graphicsFamily };
	vk::CommandPoolCreateInfo tci{ {}, m_familyIndices.transferFamily };

	m_gCommandPool = m_device->createCommandPoolUnique(gci);
	m_tCommandPool = m_device->createCommandPoolUnique(tci);
}

Window * Engine::getWindow() const
{
	return m_window.get();
}

vk::Queue Engine::getPresentQueue() const
{
	return m_graphicsQueue;
}

vk::CommandPool Engine::getWorkCommandPool() const
{
	return m_gCommandPool.get();
}

const vk::PhysicalDevice& Engine::getPhysicalDevice() const
{
	return m_physicalDevice;
}

const vk::Device& Engine::getDevice() const
{
	return m_device.get();
}
