#include <glbinding/gl40core/gl.h>
#pragma warning(disable: 4251)
#include <glbinding/Binding.h>
#pragma warning(default: 4251)

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>
#include <memory>

//Windows: Flag this executable for NVIDIA display driver - force usage of dedicated GPU
#ifdef _WIN32
extern "C"
{
	__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
}
#endif

#include "../common/marker.hpp"

#include "../common/enums.hpp"

#include "renderer/rTest/renderer_test.hpp"
#include "renderer/rA/renderer_a.hpp"
#include "renderer/rB/renderer_b.hpp"
#include "renderer/rC/renderer_c.hpp"
#include "renderer/rD/renderer_d.hpp"

void error_callback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

void GL_APIENTRY GLerror_callback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length, const gl::GLchar *message, const void *userParam)
{
	std::cerr << message << std::endl;
}

const Render::RenderType RENDERTYPE = Render::RenderType::RENDER_D;
const Render::CalcType CALCTYPE = Render::CalcType::CPU;
const Render::LineType LINETYPE = Render::LineType::GEOM;

const int WIDTH = 1280;
const int HEIGHT = 720;

std::unique_ptr<Renderer> renderer;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		else if (key == GLFW_KEY_RIGHT)
			renderer->nextEvent();
		else if (key == GLFW_KEY_LEFT)
			renderer->prevEvent();
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	renderer->handleResize(width, height);
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

int main()
{
    glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		std::cerr << "init failed" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	Marker::getInstance().nameCurrentThread("main");

	nvtxRangeId_t id1 = Marker::startRange(Marker::CAT_SETUP, 0x00FF00, __func__);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Plot draw", NULL, NULL);

    if (!window)
		std::cerr << "init failed" << std::endl;

	//Check GLFW's timer functions accuracy (on Windows)
	/*
	uint64_t frequency;

	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	printf("%d\n", frequency);
	*/

    glfwMakeContextCurrent(window);
    glfwSwapInterval(false);

    glbinding::Binding::initialize();

	if (glfwExtensionSupported("GL_KHR_debug"))
	{
		gl::ContextFlagMask ctxFlag;
		gl::glGetIntegerv(gl::GL_CONTEXT_FLAGS, (gl::GLint*)&ctxFlag);
		if ((ctxFlag & gl::GL_CONTEXT_FLAG_DEBUG_BIT) == gl::GL_CONTEXT_FLAG_DEBUG_BIT)
			gl::glDebugMessageCallback(GLerror_callback, nullptr);
	}

	switch (RENDERTYPE)
	{
		case Render::RenderType::TEST:
			renderer = std::make_unique<RendererTest>(CALCTYPE, LINETYPE);
			break;
		case Render::RenderType::RENDER_A:
			renderer = std::make_unique<RendererA>(CALCTYPE, LINETYPE);
			break;
		case Render::RenderType::RENDER_B:
			renderer = std::make_unique<RendererB>(CALCTYPE, LINETYPE);
			break;
		case Render::RenderType::RENDER_C:
			renderer = std::make_unique<RendererC>(CALCTYPE, LINETYPE);
			break;
		case Render::RenderType::RENDER_D:
			renderer = std::make_unique<RendererD>(CALCTYPE, LINETYPE);
			break;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	framebuffer_size_callback(window, WIDTH, HEIGHT);

	Marker::endRange(id1);

	const gl::GLubyte *vendor = gl::glGetString(gl::GL_RENDERER);

	std::cout << vendor << std::endl;

    auto start = std::chrono::system_clock::now();

	bool colorSwitch = false;

	uint32_t color;

	//Alternative way of measuring the frame time - queries
	//gl::GLuint queries[3];
	//gl::glGenQueries(3, queries);

	while (!glfwWindowShouldClose(window))
	{
		auto end = std::chrono::system_clock::now();

		unsigned long count = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		if (colorSwitch)
			color = Marker::COL_RED;
		else
			color = Marker::COL_BLUE;

		colorSwitch = !colorSwitch;

		nvtxRangeId_t id2 = Marker::startRange(Marker::CAT_FRAME, color, __func__);

		//gl::glBeginQuery(gl::GL_TIME_ELAPSED, queries[0]);
		//gl::glQueryCounter(queries[1], gl::GL_TIMESTAMP);
		renderer->draw(count);
		//gl::glEndQuery(gl::GL_TIME_ELAPSED);
		//gl::glQueryCounter(queries[2], gl::GL_TIMESTAMP);

		//Force GPU to do its job RIGHT NOW (otherwise it renders frames in batches, making measurement of individual frames' time impossible)
		gl::glFinish();

		//gl::GLuint val1;
		//gl::GLuint64 val2, val3;
		//gl::glGetQueryObjectuiv(queries[0], gl::GL_QUERY_RESULT_AVAILABLE, &val);
		//gl::glGetQueryObjectuiv(queries[0], gl::GL_QUERY_RESULT, &val1);
		//gl::glGetQueryObjectui64v(queries[1], gl::GL_QUERY_RESULT, &val2);
		//gl::glGetQueryObjectui64v(queries[2], gl::GL_QUERY_RESULT, &val3);

        glfwSwapBuffers(window);

		//float msf_1 = (float)val1 / 1000000;
		//float msf_2 = (float)(val3 - val2);

		//std::cout << msf_1 << " " << msf_2 << std::endl;
		//std::cout << 1000 / msf_1 << "FPS" << std::endl;

		Marker::endRange(id2);

		frameCounterCPU();

        glfwPollEvents();
    }

	//gl::glDeleteQueries(3, queries);

	gl::glFinish();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
