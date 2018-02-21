#include "track.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/lineutil.hpp"

gl::GLuint Track::s_geomProgram = 0;
gl::GLuint Track::s_hobbyProgram = 0;
gl::GLuint Track::s_passProgram = 0;

Track::Track(Render::CalcType cType, Render::LineType lType, const std::vector<glm::vec3> &pointList, glm::vec3 color) : m_cType(cType), m_lType(lType)
{
	gl::glGenVertexArrays(1, &m_VAO);

	gl::GLuint buffers[2];
	gl::glCreateBuffers(2, buffers);

	m_vertexBuffer = buffers[0];
	m_indexBuffer = buffers[1];

	std::vector<glm::vec3> vertexData;
	std::vector<gl::GLushort> indexData;

	LineUtil::fillBuffers(cType, lType, pointList, vertexData, indexData);

	m_count = (gl::GLsizei)indexData.size();

	gl::glNamedBufferData(m_indexBuffer, m_count * sizeof(gl::GLushort), indexData.data(), gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), vertexData.data(), gl::GL_STATIC_DRAW);

	gl::glBindVertexArray(m_VAO);
	gl::glEnableVertexAttribArray(Track::ATTRIB_POS);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vertexBuffer);
	gl::glVertexAttribPointer(Track::ATTRIB_POS, 3, gl::GL_FLOAT, gl::GL_FALSE, 0, nullptr);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);

	gl::glBindVertexArray(0);

	//2x big
	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));
	m_color = color;
}

Track::~Track()
{
	gl::glDeleteBuffers(1, &m_vertexBuffer);
	gl::glDeleteBuffers(1, &m_indexBuffer);
}

void Track::draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	gl::glBindVertexArray(m_VAO);

	if (m_cType == Render::CalcType::CPU)
		gl::glUseProgram(s_passProgram);
	else if (m_lType == Render::LineType::GEOM)
		gl::glUseProgram(s_geomProgram);
	else
		gl::glUseProgram(s_hobbyProgram);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, glm::value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(camPos));
	gl::glUniform3fv(2, 1, value_ptr(m_color));

	gl::glDrawElements(gl::GL_PATCHES, m_count, gl::GL_UNSIGNED_SHORT, nullptr);

	gl::glUseProgram(0);
	gl::glBindVertexArray(0);
}

void Track::createPrograms()
{
	gl::GLuint vertShader, controlShader, evalShader, fragShader;

	// -- Shared shaders -- //
	vertShader = loadCompileShader(gl::GL_VERTEX_SHADER, "uniform_col.vert");
	fragShader = loadCompileShader(gl::GL_FRAGMENT_SHADER, "color.frag");
	evalShader = loadCompileShader(gl::GL_TESS_EVALUATION_SHADER, "bezier.tese");

	// -- Geometric control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "geom.tesc");

	Track::s_geomProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Hobby control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "hobby.tesc");

	Track::s_hobbyProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Passthrough (no evaluation, points from a buffer) -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "pass.tesc");

	Track::s_passProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	gl::glDeleteShader(evalShader);

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);
}

void Track::destroyPrograms()
{
	gl::glDeleteProgram(Track::s_geomProgram);
	gl::glDeleteProgram(Track::s_hobbyProgram);
	gl::glDeleteProgram(Track::s_passProgram);
}
