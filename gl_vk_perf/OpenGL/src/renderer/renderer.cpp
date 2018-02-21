#include "renderer.hpp"

#include <glbinding/gl45core/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void Renderer::handleResize(int width, int height)
{
	gl::glViewport(0, 0, width, height);
	float aspectRatio = (float)width / (float)height;
	m_projection = glm::perspective(glm::radians(45.f), aspectRatio, .1f, 100.f);
};