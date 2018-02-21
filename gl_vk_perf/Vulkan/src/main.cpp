#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "engine.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"

#include "../common/enums.hpp"

#include "renderer/rA/renderer_a.hpp"
#include "renderer/rB/renderer_b.hpp"
#include "renderer/rC/renderer_c.hpp"
#include "renderer/rD/renderer_d.hpp"

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

std::unique_ptr<Renderer> renderer;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void frameCounterCPU()
{
	static double lastTime = glfwGetTime();
	static int nbFrames = 0;

	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 10.0)
	{
		std::cout << 10000.0 / double(nbFrames) << " ms/frame" << std::endl;
		std::cout << double(nbFrames) / 10.0 << " FPS" << std::endl;
		nbFrames = 0;
		lastTime += 10.0;
	}
}

const Render::RenderType RENDERTYPE = Render::RenderType::RENDER_A;
const Render::CalcType CALCTYPE = Render::CalcType::GPU;
const Render::LineType LINETYPE = Render::LineType::HOBBY;
const Render::RecordType RECTYPE = Render::RecordType::DYNAMIC;

const int WIDTH = 1280;
const int HEIGHT = 720;

int main()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		std::cerr << "init failed" << std::endl;

	Engine &e = Engine::getInstance();

	e.createWindow(WIDTH, HEIGHT);
	e.selectPhysicalDevice();
	e.createLogicalDevice();
	std::unique_ptr<Swapchain> swapchain = std::make_unique<Swapchain>();

	switch (RENDERTYPE)
	{
		case Render::RenderType::RENDER_A:
			renderer = std::make_unique<RendererA>(*swapchain.get(), CALCTYPE, LINETYPE, RECTYPE);
			break;

		case Render::RenderType::RENDER_B:
			renderer = std::make_unique<RendererB>(*swapchain.get(), CALCTYPE, LINETYPE, RECTYPE);
			break;

		case Render::RenderType::RENDER_C:
			renderer = std::make_unique<RendererC>(*swapchain.get(), CALCTYPE, LINETYPE, RECTYPE);
			break;
		case Render::RenderType::RENDER_D:
			renderer = std::make_unique<RendererD>(*swapchain.get(), CALCTYPE, LINETYPE, RECTYPE);
			break;
	}

	glfwSetKeyCallback(e.getWindow()->getWindow(), key_callback);

	{
		vk::SemaphoreCreateInfo sci{};
		vk::UniqueSemaphore imageAvailableSemaphore = e.getDevice().createSemaphoreUnique(sci);
		vk::UniqueSemaphore renderFinishedSemaphore = e.getDevice().createSemaphoreUnique(sci);

		auto start = std::chrono::system_clock::now();

		while (!e.shouldClose())
		{
			e.getPresentQueue().waitIdle();

			frameCounterCPU();

			glfwPollEvents();

			auto end = std::chrono::system_clock::now();

			unsigned long count = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			try {
				auto acquireResult = e.getDevice().acquireNextImageKHR(swapchain->getSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore.get(), vk::Fence());

				uint32_t imageIndex = acquireResult.value;

				vk::CommandBuffer cmd = renderer->draw(count, imageIndex);

				vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
				vk::SubmitInfo submitInfo{ 1, &imageAvailableSemaphore.get(), &waitStages, 1, &cmd, 1, &renderFinishedSemaphore.get() };

				e.getPresentQueue().submit(1, &submitInfo, vk::Fence());

				vk::PresentInfoKHR presentInfo{
					1, &renderFinishedSemaphore.get(),
					1, &swapchain->getSwapChain(),
					&imageIndex, nullptr
				};

				e.getPresentQueue().presentKHR(presentInfo);
			}
			catch (const vk::OutOfDateKHRError &)
			{
				e.getDevice().waitIdle();
				renderer->freeSwapchainDependent();

				swapchain.reset();
				swapchain = std::make_unique<Swapchain>();

				renderer->initSwapchainDependent(*swapchain.get());
			}
		}
		e.getDevice().waitIdle();
	}

	renderer.reset();
	swapchain.reset();

	Engine::resetInstance();

	glfwTerminate();

	return 0;
}