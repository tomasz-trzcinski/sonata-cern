#include "tst_line_draw.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <utility>
#include <iostream>

#include "../common/lineutil.hpp"

TestLineDraw::TestLineDraw(Render::CalcType cType, Render::LineType lType) : m_cType(cType), m_lType(lType)
{
	gl::glGenVertexArrays(1, &this->VAO);

	gl::GLuint buffers[2];
	gl::glGenBuffers(2, buffers);

	this->vertexBuffer = buffers[0];
	this->indexBuffer = buffers[1];

	//Initialize the generated buffer
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, this->vertexBuffer);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);

	gl::glBindVertexArray(this->VAO);

	// -- Attrib 0 -> position -- //
	gl::glEnableVertexAttribArray(TestLineDraw::ATTRIB_POS);
	gl::glVertexAttribFormat(TestLineDraw::ATTRIB_POS, 3, gl::GL_FLOAT, gl::GL_FALSE, 0);

	gl::glBindVertexBuffer(TestLineDraw::POS_STREAM, this->vertexBuffer, 0, sizeof(glm::vec3));
	gl::glVertexAttribBinding(TestLineDraw::ATTRIB_POS, TestLineDraw::POS_STREAM);
	
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);

	gl::glBindVertexArray(0);

	gl::GLuint vertShader, controlShader, evalShader, fragShader;

	// -- Shared shaders -- //
	vertShader = loadCompileShader(gl::GL_VERTEX_SHADER, "uniform_col.vert");
	fragShader = loadCompileShader(gl::GL_FRAGMENT_SHADER, "color.frag");
	evalShader = loadCompileShader(gl::GL_TESS_EVALUATION_SHADER, "bezier.tese");

	// -- Geometric control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "geom.tesc");

	this->bezierProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Hobby control point evaluation -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "hobby.tesc");

	this->hobbyProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	// -- Passthrough (no evaluation, points from buffer) -- //
	controlShader = loadCompileShader(gl::GL_TESS_CONTROL_SHADER, "pass.tesc");

	this->passProgram = createProgram({ vertShader, controlShader, evalShader, fragShader });
	gl::glDeleteShader(controlShader);

	gl::glDeleteShader(evalShader);

	// -- Generic -- //
	this->genericProgram = createProgram({ vertShader, fragShader });

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);
	
	setupData();

	this->Model = glm::scale(glm::mat4(1.f), glm::vec3(0.01f));
}

TestLineDraw::~TestLineDraw()
{
	gl::glDeleteProgram(this->genericProgram);
	gl::glDeleteProgram(this->bezierProgram);
	gl::glDeleteProgram(this->hobbyProgram);

	gl::glDeleteVertexArrays(1, &this->VAO);
	gl::glDeleteBuffers(1, &this->vertexBuffer);
	gl::glDeleteBuffers(1, &this->indexBuffer);
}

void TestLineDraw::draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP)
{
	gl::glPatchParameteri(gl::GL_PATCH_VERTICES, TestLineDraw::PATCH_SIZE);

	glm::mat4 MVP = VP * this->Model;

	gl::glBindVertexArray(this->VAO);

	MVP = glm::mat4(1.0f);
	glm::vec3 col(1.0f, 0.0f, 0.0f);

	if (m_cType == Render::CalcType::CPU)
		gl::glUseProgram(this->passProgram);
	else if(m_lType == Render::LineType::GEOM)
		gl::glUseProgram(this->bezierProgram);
	else
		gl::glUseProgram(this->hobbyProgram);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, glm::value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(camPos));
	gl::glUniform3fv(2, 1, value_ptr(col));

	gl::glDrawElements(gl::GL_PATCHES, 12, gl::GL_UNSIGNED_SHORT, nullptr);

	gl::glUseProgram(this->genericProgram);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, glm::value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(camPos));
	gl::glUniform3fv(2, 1, value_ptr(col));

	if (m_cType != Render::CalcType::CPU)
		gl::glDrawArrays(gl::GL_LINE_STRIP, 0, 4);

	gl::glPointSize(8);
	gl::glDrawArrays(gl::GL_POINTS, m_startOffset, 6);
}

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

void TestLineDraw::setupData()
{
	std::vector<glm::vec3> vertices { { 0.0f, 0.5f, 0.0f },{ 0.5f, 0.0f, 0.0f } ,{ 0.0f, -0.5f, 0.0f },{ -0.5f, 0.0f, 0.0f } };

	std::vector<glm::vec3> vData;
	std::vector<gl::GLushort> iData;

	if (m_cType == Render::CalcType::CPU)
	{
		glm::vec3 c0, c1, c2, c3, c4, c5;
		if (m_lType == Render::LineType::GEOM)
		{
			glm::vec3 dummy;

			std::tie(dummy, c0) = LineUtil::calcGeomControlPoints(vertices[0], vertices[0], vertices[1]);
			std::tie(c1, dummy) = LineUtil::calcGeomControlPoints(vertices[0], vertices[1], vertices[2]);

			std::tie(dummy, c2) = LineUtil::calcGeomControlPoints(vertices[0], vertices[1], vertices[2]);
			std::tie(c3, dummy) = LineUtil::calcGeomControlPoints(vertices[1], vertices[2], vertices[3]);

			std::tie(dummy, c4) = LineUtil::calcGeomControlPoints(vertices[1], vertices[2], vertices[3]);
			std::tie(c5, dummy) = LineUtil::calcGeomControlPoints(vertices[2], vertices[3], vertices[3]);
		}
		else
		{
			std::tie(c0, c1) = LineUtil::calcHobbyControlPoints(vertices[0], vertices[0], vertices[1], vertices[2]);
			std::tie(c2, c3) = LineUtil::calcHobbyControlPoints(vertices[0], vertices[1], vertices[2], vertices[3]);
			std::tie(c4, c5) = LineUtil::calcHobbyControlPoints(vertices[1], vertices[2], vertices[3], vertices[3]);
		}

		vData.push_back(vertices[0]);
		vData.push_back(c0);
		vData.push_back(c1);

		vData.push_back(vertices[1]);
		vData.push_back(c2);
		vData.push_back(c3);

		vData.push_back(vertices[2]);
		vData.push_back(c4);
		vData.push_back(c5);

		vData.push_back(vertices[3]);

		iData.push_back(0);
		iData.push_back(1);
		iData.push_back(2);
		iData.push_back(3);

		iData.push_back(3);
		iData.push_back(4);
		iData.push_back(5);
		iData.push_back(6);

		iData.push_back(6);
		iData.push_back(7);
		iData.push_back(8);
		iData.push_back(9);

		vData.push_back(c0);
		vData.push_back(c1);
		vData.push_back(c2);
		vData.push_back(c3);
		vData.push_back(c4);
		vData.push_back(c5);

		m_startOffset = 10;
	}
	else
	{
		vData = vertices;

		iData = {
			0, 0, 1, 2,
			0, 1, 2, 3,
			1, 2, 3, 3,
		};

		glm::vec3 c0, c1, c2, c3, c4, c5;

		if (m_lType == Render::LineType::GEOM)
		{
			glm::vec3 dummy;

			std::tie(dummy, c0) = LineUtil::calcGeomControlPoints(vertices[0], vertices[0], vertices[1]);
			std::tie(c1, dummy) = LineUtil::calcGeomControlPoints(vertices[0], vertices[1], vertices[2]);

			std::tie(dummy, c2) = LineUtil::calcGeomControlPoints(vertices[0], vertices[1], vertices[2]);
			std::tie(c3, dummy) = LineUtil::calcGeomControlPoints(vertices[1], vertices[2], vertices[3]);

			std::tie(dummy, c4) = LineUtil::calcGeomControlPoints(vertices[1], vertices[2], vertices[3]);
			std::tie(c5, dummy) = LineUtil::calcGeomControlPoints(vertices[2], vertices[3], vertices[3]);
		}
		else
		{
			std::tie(c0, c1) = LineUtil::calcHobbyControlPoints(vertices[0], vertices[0], vertices[1], vertices[2]);
			std::tie(c2, c3) = LineUtil::calcHobbyControlPoints(vertices[0], vertices[1], vertices[2], vertices[3]);
			std::tie(c4, c5) = LineUtil::calcHobbyControlPoints(vertices[1], vertices[2], vertices[3], vertices[3]);
		}

		vData.push_back(c0);
		vData.push_back(c1);
		vData.push_back(c2);
		vData.push_back(c3);
		vData.push_back(c4);
		vData.push_back(c5);

		m_startOffset = 4;
	}

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, this->vertexBuffer);
	gl::glBufferData(gl::GL_ARRAY_BUFFER, vData.size() * sizeof(glm::vec3), &vData[0], gl::GL_STATIC_DRAW);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);

	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, iData.size() * sizeof(gl::GLushort), &iData[0], gl::GL_STATIC_DRAW);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, 0);
}