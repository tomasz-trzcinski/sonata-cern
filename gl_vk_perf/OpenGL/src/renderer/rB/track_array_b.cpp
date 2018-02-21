#include "track_array_b.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <utility>

#include "../common/lineutil.hpp"

TrackArrayB::TrackArrayB(Render::CalcType cType, Render::LineType lType) : m_cType(cType), m_lType(lType)
{
	m_eventMgr = std::make_unique<AliEventManager>();

	gl::glGenVertexArrays(1, &m_VAO);

	gl::GLuint buffers[2];
	gl::glCreateBuffers(2, buffers);

	m_vertexBuffer = buffers[0];
	m_indexBuffer = buffers[1];

	gl::glBindVertexArray(m_VAO);
	gl::glEnableVertexAttribArray(TrackArrayB::ATTRIB_POS);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vertexBuffer);
	gl::glVertexAttribPointer(TrackArrayB::ATTRIB_POS, 3, gl::GL_FLOAT, gl::GL_FALSE, 0, nullptr);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);

	gl::glBindVertexArray(0);

	gl::GLuint vertShader, controlShader, evalShader, fragShader;

	// -- Shared shaders -- //
	vertShader = loadCompileShader(gl::GL_VERTEX_SHADER, "uniform_col.vert");
	fragShader = loadCompileShader(gl::GL_FRAGMENT_SHADER, "color.frag");
	evalShader = loadCompileShader(gl::GL_TESS_EVALUATION_SHADER, "bezier.tese");

	// -- Geometric control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "geom.tesc");

	m_geomProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Hobby control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "hobby.tesc");

	m_hobbyProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Generic -- //
	//this->genericProgram = createProgram({ vertShader, fragShader });
	// -- Passthrough (no evaluation, points from buffer) -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "pass.tesc");

	m_passProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	gl::glDeleteShader(evalShader);

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);

	//2x bigger
	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));

	loadTracks();
}

TrackArrayB::~TrackArrayB()
{
	gl::glDeleteProgram(m_geomProgram);
	gl::glDeleteProgram(m_hobbyProgram);
	gl::glDeleteProgram(m_passProgram);

	gl::glDeleteVertexArrays(1, &m_VAO);
	gl::glDeleteBuffers(1, &m_vertexBuffer);
	gl::glDeleteBuffers(1, &m_indexBuffer);
}

void TrackArrayB::draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	gl::glBindVertexArray(m_VAO);

	if (m_cType == Render::CalcType::CPU)
		gl::glUseProgram(m_passProgram);
	else if (m_lType == Render::LineType::GEOM)
		gl::glUseProgram(m_geomProgram);
	else
		gl::glUseProgram(m_hobbyProgram);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, glm::value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(camPos));
	
	for(const auto& pList : m_renderList)
	{
		gl::glUniform3fv(2, 1, value_ptr(pList.second.color));
		for(const auto &particle : pList.second.trackInfos)
		{
			gl::glDrawElementsBaseVertex(gl::GL_PATCHES, particle.count, gl::GL_UNSIGNED_SHORT, particle.indexOffset, particle.vertexOffset);
		}
	}
}

void TrackArrayB::prevEvent()
{
	m_eventMgr->prevEvent();

	loadTracks();
}

void TrackArrayB::nextEvent()
{
	m_eventMgr->nextEvent();

	loadTracks();
}

void TrackArrayB::loadTracks()
{
	gl::glBindVertexArray(0);

	m_renderList.clear();

	std::vector<glm::vec3> vertexData;
	std::vector<gl::GLushort> indexData;

	for (auto const &track : m_eventMgr->m_tracks)
	{
		if (track.m_pointList.size() > 2)
		{
			size_t vertexOffset = vertexData.size();
			size_t indexStart = indexData.size();

			LineUtil::fillBuffers(m_cType, m_lType, track.m_pointList, vertexData, indexData);

			size_t count = indexData.size() - indexStart;

			auto info = m_renderList.find(track.m_PID);

			gl::GLushort* indexOffset = nullptr;

			TrackInfo tInfo { &indexOffset[indexStart], (gl::GLint)vertexOffset, (gl::GLsizei)count };

			if (info == m_renderList.end())
			{
				m_renderList.emplace(std::pair<int32_t, ParticleList>(track.m_PID, { glm::sphericalRand(1.0f), { tInfo } }));
			}
			else
			{
				info->second.trackInfos.push_back(tInfo);
			}
		}
	}

	gl::glNamedBufferData(m_indexBuffer, indexData.size() * sizeof(gl::GLushort), &indexData[0], gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), &vertexData[0], gl::GL_STATIC_DRAW);
}
