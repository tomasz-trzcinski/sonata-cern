#include "track_array_c.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <utility>
#include <unordered_map>
#include <tuple>

#include "../common/lineutil.hpp"

#include <iostream>

TrackArrayC::TrackArrayC(Render::CalcType cType, Render::LineType lType) : m_cType(cType), m_lType(lType)
{
	m_eventMgr = std::make_unique<AliEventManager>();

	gl::glGenVertexArrays(1, &m_VAO);

	gl::GLuint buffers[3];
	gl::glCreateBuffers(3, buffers);

	m_vertexBuffer = buffers[0];
	m_indexBuffer = buffers[1];
	m_storageBuffer = buffers[2];

	gl::glBindVertexArray(m_VAO);
	gl::glEnableVertexAttribArray(TrackArrayC::ATTRIB_POS);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vertexBuffer);
	gl::glVertexAttribPointer(TrackArrayC::ATTRIB_POS, 3, gl::GL_FLOAT, gl::GL_FALSE, 0, nullptr);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
	gl::glBindVertexArray(0);

	gl::GLuint vertShader, controlShader, evalShader, fragShader;

	// -- Shared shaders -- //
	vertShader = loadCompileShader(gl::GL_VERTEX_SHADER, "drawid_col.vert");
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

TrackArrayC::~TrackArrayC()
{
	gl::glDeleteProgram(m_geomProgram);
	gl::glDeleteProgram(m_hobbyProgram);
	gl::glDeleteProgram(m_passProgram);

	gl::glDeleteVertexArrays(1, &m_VAO);
	gl::glDeleteBuffers(1, &m_vertexBuffer);
	gl::glDeleteBuffers(1, &m_indexBuffer);
	gl::glDeleteBuffers(1, &m_storageBuffer);
}

void TrackArrayC::draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	gl::glBindVertexArray(m_VAO);

	glm::vec3 col(1.0f, 0.0f, 0.0f);

	if (m_cType == Render::CalcType::CPU)
		gl::glUseProgram(m_passProgram);
	else if (m_lType == Render::LineType::GEOM)
		gl::glUseProgram(m_geomProgram);
	else
		gl::glUseProgram(m_hobbyProgram);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, glm::value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(camPos));
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 0, m_storageBuffer);

	gl::GLsizei drawCount = (gl::GLsizei)m_counts.size();

	gl::glMultiDrawElementsBaseVertex(gl::GL_PATCHES, m_counts.data(), gl::GL_UNSIGNED_SHORT, m_indexOffsets.data(), drawCount, m_vertexOffsets.data());
}

void TrackArrayC::prevEvent()
{
	m_eventMgr->prevEvent();

	loadTracks();
}

void TrackArrayC::nextEvent()
{
	m_eventMgr->nextEvent();

	loadTracks();
}

void TrackArrayC::loadTracks()
{
	gl::glBindVertexArray(0);

	m_counts.clear();
	m_indexOffsets.clear();
	m_vertexOffsets.clear();

	std::unordered_map<int32_t, glm::vec3> pList;

	std::vector<glm::vec3> vertexData;
	std::vector<gl::GLushort> indexData;
	std::vector<glm::vec3> colorData;

	for (auto const &track : m_eventMgr->m_tracks)
	{
		if (track.m_pointList.size() > 2)
		{
			gl::GLint vertexOffset = (gl::GLint)vertexData.size();
			size_t indexStart = indexData.size();

			LineUtil::fillBuffers(m_cType, m_lType, track.m_pointList, vertexData, indexData);

			gl::GLsizei count = (gl::GLsizei)(indexData.size() - indexStart);

			gl::GLushort* indexOffset = nullptr;

			m_counts.push_back(count);
			m_indexOffsets.push_back(&indexOffset[indexStart]);
			m_vertexOffsets.push_back(vertexOffset);

			auto key = pList.find(track.m_PID);

			if (key == pList.end())
			{
				glm::vec3 color = glm::sphericalRand(1.0f);
				pList.emplace(std::pair<int32_t, glm::vec3>(track.m_PID, color));
				colorData.push_back(color);
			}
			else
			{
				colorData.push_back(key->second);
			}
		}
	}

	gl::glNamedBufferData(m_indexBuffer, indexData.size() * sizeof(gl::GLushort), &indexData[0], gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), &vertexData[0], gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(m_storageBuffer, colorData.size() * sizeof(glm::vec3), &colorData[0], gl::GL_STATIC_DRAW);
}
