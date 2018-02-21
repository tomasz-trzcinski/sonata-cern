#include "swapchain.hpp"

#include <iostream>

Swapchain::Swapchain(uint32_t images) : m_engine(Engine::getInstance())
{
	m_format = chooseSurfaceFormat();
	m_presentMode = choosePresentMode();
	m_extent = chooseExtent();

	const vk::SurfaceKHR &surf = m_engine.getWindow()->getSurface();
	const vk::SurfaceCapabilitiesKHR capabilities = m_engine.getPhysicalDevice().getSurfaceCapabilitiesKHR(surf);

	uint32_t imageCount = capabilities.minImageCount + 5; //std::max<uint32_t>(images, capabilities.minImageCount);

	vk::SwapchainCreateInfoKHR swci{ {}, surf, imageCount, m_format.format, m_format.colorSpace, 
		m_extent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, 
		capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, m_presentMode, VK_TRUE, {} };
	
	m_swapChain = m_engine.getDevice().createSwapchainKHRUnique(swci);

	m_swapChainImages = m_engine.getDevice().getSwapchainImagesKHR(m_swapChain.get());

	m_swapChainImageViews.reserve(m_swapChainImages.size());

	vk::ImageSubresourceRange range{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
	for (const auto &image : m_swapChainImages)
	{
		vk::ImageViewCreateInfo iwci{ {}, image, vk::ImageViewType::e2D, m_format.format, {}, range };
		m_swapChainImageViews.push_back(m_engine.getDevice().createImageViewUnique(iwci));
	}

	vk::Format depthFormat = chooseDepthFormat();

	range.aspectMask = vk::ImageAspectFlagBits::eDepth;

	if (hasStencilComponent(depthFormat))
		range.aspectMask |= vk::ImageAspectFlagBits::eStencil;

	m_engine.createImage(m_depthImageMemory, m_depthImage, m_extent.width, m_extent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);

	vk::ImageViewCreateInfo iwci{ {}, m_depthImage.get(), vk::ImageViewType::e2D, chooseDepthFormat(), {}, { vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 } };
	m_depthImageView = m_engine.getDevice().createImageViewUnique(iwci);

	m_engine.transitionImageLayout(m_depthImage.get(), depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, range);
}

vk::Extent2D Swapchain::chooseExtent() const
{
	const vk::SurfaceKHR &surf = m_engine.getWindow()->getSurface();
	const vk::SurfaceCapabilitiesKHR capabilities = m_engine.getPhysicalDevice().getSurfaceCapabilitiesKHR(surf);

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D window = m_engine.getWindow()->getWindowSize();
		return window;
	}
}

vk::PresentModeKHR Swapchain::choosePresentMode() const
{
	const vk::SurfaceKHR &surf = m_engine.getWindow()->getSurface();
	const std::vector<vk::PresentModeKHR> availablePresentModes = m_engine.getPhysicalDevice().getSurfacePresentModesKHR(surf);

	vk::PresentModeKHR fallbackMode = vk::PresentModeKHR::eFifo;

	for (const auto &mode : availablePresentModes)
	{
		if (mode == vk::PresentModeKHR::eMailbox)
		{
			return mode;
		}
		else if (mode == vk::PresentModeKHR::eImmediate)
		{
			fallbackMode = mode;
		}
	}
	
	return fallbackMode;
}

vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat() const
{
	const vk::SurfaceKHR &surf = m_engine.getWindow()->getSurface();
	const std::vector<vk::SurfaceFormatKHR> availableFormats = m_engine.getPhysicalDevice().getSurfaceFormatsKHR(surf);

	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
	{
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto &availableFormat : availableFormats)
	{
		//std::cout << "Format: " << vk::to_string(availableFormat.format) << " " << vk::to_string(availableFormat.colorSpace) << std::endl;
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::Format Swapchain::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for (const auto &format : candidates)
	{
		vk::FormatProperties props = m_engine.getPhysicalDevice().getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format!");
}

bool Swapchain::hasStencilComponent(const vk::Format format) const
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

vk::Format Swapchain::chooseDepthFormat() const
{
	return findSupportedFormat(
		{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, 
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::Extent2D Swapchain::getExtent() const
{
	return m_extent;
}

vk::Rect2D Swapchain::getRenderArea() const
{
	return { { 0, 0 }, m_extent };
}

size_t Swapchain::getNumImages() const
{
	return m_swapChainImages.size();
}

vk::ImageView Swapchain::getImageView(size_t index) const
{
	return m_swapChainImageViews[index].get();
}

vk::ImageView Swapchain::getDepthView() const
{
	return m_depthImageView.get();
}

vk::SwapchainKHR Swapchain::getSwapChain() const
{
	return m_swapChain.get();
}