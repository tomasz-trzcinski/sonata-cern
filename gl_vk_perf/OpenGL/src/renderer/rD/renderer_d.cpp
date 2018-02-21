#include "renderer_d.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

RendererD::RendererD(Render::CalcType cType, Render::LineType lType)
{
	m_tArray = std::make_unique<TrackArrayD>(cType, lType);

	gl::glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	gl::glClearDepth(1.0f);

	gl::glPatchParameteri(gl::GL_PATCH_VERTICES, RendererD::PATCH_SIZE);
	gl::glEnable(gl::GL_DEPTH_TEST);
}

RendererD::~RendererD()
{

}

void RendererD::draw(unsigned long duration)
{
	float angle = (duration / 15000.0f) * 2 * glm::pi<float>();

	float radius = 20.f;

	glm::vec3 cameraPos(radius * glm::sin(angle), radius, radius * glm::cos(angle));

	m_view = glm::lookAt(cameraPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	glm::mat4 VP = m_projection * m_view;

	gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

	m_tArray->draw(duration, cameraPos, VP);
}

void RendererD::prevEvent()
{
	m_tArray->prevEvent();
}

void RendererD::nextEvent()
{
	m_tArray->nextEvent();
}
