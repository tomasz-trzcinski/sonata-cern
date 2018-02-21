#include "renderer.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() : m_engine(Engine::getInstance())
{
}

void Renderer::initSwapchainDependent(const Swapchain &swapchain)
{
	m_renderArea = swapchain.getRenderArea();
	float aspectRatio = (float)m_renderArea.extent.width / (float)m_renderArea.extent.height;
	m_projection = glm::perspective(glm::radians(45.f), aspectRatio, .1f, 100.f);
	m_projection[1][1] *= -1;
}