#ifndef __H_SWAPCHAIN__
#define __H_SWAPCHAIN__

#include "engine.hpp"

class Swapchain
{
	private:
		Engine & m_engine;

		vk::SurfaceFormatKHR m_format;
		vk::PresentModeKHR m_presentMode;
		vk::Extent2D m_extent;

		vk::UniqueSwapchainKHR m_swapChain;
		std::vector<vk::Image> m_swapChainImages;
		std::vector<vk::UniqueImageView> m_swapChainImageViews;

		vk::UniqueDeviceMemory m_depthImageMemory;
		vk::UniqueImage m_depthImage;
		vk::UniqueImageView m_depthImageView;

		vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
		bool hasStencilComponent(const vk::Format format) const;
	public:
		Swapchain(uint32_t images = 2);

		vk::Extent2D chooseExtent() const;
		vk::PresentModeKHR choosePresentMode() const;
		vk::SurfaceFormatKHR chooseSurfaceFormat() const;
		vk::Format chooseDepthFormat() const;

		vk::Extent2D getExtent() const;
		vk::Rect2D getRenderArea() const;
		size_t getNumImages() const;
		vk::ImageView getImageView(size_t index) const;
		vk::ImageView getDepthView() const;

		vk::SwapchainKHR getSwapChain() const;
};

#endif