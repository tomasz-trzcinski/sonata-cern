#ifndef __H_RENDERER__
#define __H_RENDERER__

#include <glm/matrix.hpp>

#include "../engine.hpp"
#include "../swapchain.hpp"

class Renderer
{
	protected:
		Engine &m_engine;
		vk::Rect2D m_renderArea;

		glm::mat4 m_projection;

	public:
		Renderer();
		virtual ~Renderer() {}

		virtual vk::CommandBuffer draw(unsigned long duration, uint32_t imageIndex) = 0;

		virtual void prevEvent() = 0;
		virtual void nextEvent() = 0;

		virtual void initSwapchainDependent(const Swapchain &swapchain);
		virtual void freeSwapchainDependent() = 0;
};

#endif