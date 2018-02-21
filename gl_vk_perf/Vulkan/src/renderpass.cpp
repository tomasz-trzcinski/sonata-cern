#include "renderpass.hpp"

Renderpass::Renderpass(const Swapchain &swapchain) : m_engine(Engine::getInstance())
{
	vk::Format colorFormat = swapchain.chooseSurfaceFormat().format;
	vk::Format depthFormat = swapchain.chooseDepthFormat();

	vk::AttachmentDescription color{ {}, colorFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
	};

	vk::AttachmentDescription depth{ {}, depthFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal
	};

	std::vector<vk::AttachmentDescription> colorAttachements{ color };

	std::vector<vk::AttachmentDescription> renderPassAttachements;

	std::vector<vk::AttachmentReference> colorAttachementRefs;

	for (const auto &cAttachment : colorAttachements)
	{
		uint32_t index = uint32_t(renderPassAttachements.size());
		colorAttachementRefs.push_back({ index, vk::ImageLayout::eColorAttachmentOptimal });
		renderPassAttachements.push_back(cAttachment);
	}

	uint32_t index = uint32_t(renderPassAttachements.size());
	renderPassAttachements.push_back(depth);

	vk::AttachmentReference depthAttachementRef{ index, vk::ImageLayout::eDepthStencilAttachmentOptimal };

	vk::SubpassDescription subpass0{ {}, vk::PipelineBindPoint::eGraphics,
		0, nullptr,
		uint32_t(colorAttachementRefs.size()), colorAttachementRefs.data(),
		nullptr, &depthAttachementRef,
		0, nullptr,
	};

	vk::SubpassDependency dependency_imageAcquire{ 
		VK_SUBPASS_EXTERNAL, 0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		{}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
		{} 
	};

	std::vector<vk::SubpassDescription> subpasses{ subpass0 };
	std::vector<vk::SubpassDependency> dependencies{ dependency_imageAcquire };

	vk::RenderPassCreateInfo rpci{ {},
		uint32_t(renderPassAttachements.size()), renderPassAttachements.data(),
		uint32_t(subpasses.size()), subpasses.data(),
		uint32_t(dependencies.size()), dependencies.data()
	};

	m_renderpass = m_engine.getDevice().createRenderPassUnique(rpci);

	m_framebuffers.reserve(swapchain.getNumImages());

	for (size_t i = 0; i < swapchain.getNumImages(); ++i)
	{
		std::vector<vk::ImageView> fbAttachments{ swapchain.getImageView(i), swapchain.getDepthView() };

		vk::FramebufferCreateInfo fbci{ {}, m_renderpass.get(),
			uint32_t(fbAttachments.size()), fbAttachments.data(),
			swapchain.getExtent().width, swapchain.getExtent().height,
			1
		};

		m_framebuffers.push_back(m_engine.getDevice().createFramebufferUnique(fbci));
	}
}

const vk::RenderPass Renderpass::getPass() const
{
	return m_renderpass.get();
}

const vk::Framebuffer Renderpass::getFramebuffer(size_t index) const
{
	return m_framebuffers[index].get();
}
