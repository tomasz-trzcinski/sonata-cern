#ifndef __H_RENDERPASS__
#define __H_RENDERPASS__

#include "engine.hpp"
#include "swapchain.hpp"

class Renderpass
{
	private:
		Engine & m_engine;

		vk::UniqueRenderPass m_renderpass;
		std::vector<vk::UniqueFramebuffer> m_framebuffers;

	public:
		Renderpass(const Swapchain &swapchain);
		const vk::RenderPass getPass() const;
		const vk::Framebuffer getFramebuffer(size_t index) const;
};

#endif