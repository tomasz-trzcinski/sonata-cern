#include "renderer_a.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

RendererA::RendererA(Render::CalcType cType, Render::LineType lType) : m_cType(cType), m_lType(lType)
{
	m_eventMgr = std::make_unique<AliEventManager>();
	m_model = std::make_unique<Model>();

	gl::glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	gl::glClearDepth(1.0f);

	gl::glPatchParameteri(gl::GL_PATCH_VERTICES, RendererA::PATCH_SIZE);
	gl::glEnable(gl::GL_DEPTH_TEST);

	Track::createPrograms();

	loadTracks();
}

RendererA::~RendererA()
{
	Track::destroyPrograms();
}

void RendererA::draw(unsigned long duration)
{
	float angle = (duration / 15000.0f) * 2 * glm::pi<float>();

	float radius = 20.f;

	glm::vec3 cameraPos(radius * glm::sin(angle), radius, radius * glm::cos(angle));

	m_view = glm::lookAt(cameraPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	glm::mat4 VP = m_projection * m_view;

	gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

	//m_model->draw(duration, cameraPos, VP);

	for (const auto &track : m_tracks)
		track->draw(duration, cameraPos, VP);
}

void RendererA::loadTracks()
{
	m_tracks.clear();
	for (auto const &track : m_eventMgr->m_tracks)
	{
		if (track.m_pointList.size() > 2)
			m_tracks.push_back(std::make_unique<Track>(m_cType, m_lType, track.m_pointList, glm::sphericalRand(1.0f)));
	}
}

void RendererA::prevEvent()
{
	m_eventMgr->prevEvent();

	loadTracks();
}

void RendererA::nextEvent()
{
	m_eventMgr->nextEvent();

	loadTracks();
}
