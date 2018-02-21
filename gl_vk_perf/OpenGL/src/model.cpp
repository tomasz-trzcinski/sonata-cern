#include "model.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../common/marker.hpp"

Model::Model()
{
	auto lMark = MarkerScoped(Marker::CAT_LOADING, Marker::COL_GREEN, "load_model");
	m_modelMgr = std::make_unique<AliModelManager>("alicegeom.obj");
	gl::GLuint buffers[2];
	gl::glCreateBuffers(2, buffers);

	m_vertexBuffer = buffers[0];
	m_indexBuffer = buffers[1];

	std::vector<glm::vec3> vBufferCombined;
	std::vector<glm::u16vec3> iBufferCombined;

	for (const AliModelManager::Mesh &mesh : m_modelMgr->m_meshes)
	{
		vBufferCombined.insert(vBufferCombined.end(), mesh.m_vertices.begin(), mesh.m_vertices.end());
		m_indexOffsets.push_back(iBufferCombined.size());
		m_drawSizes.push_back((gl::GLsizei)(mesh.m_faces.size() * 3));
		//Convert from unsigned ints to shorts, takes half as much memory
		iBufferCombined.insert(iBufferCombined.end(), mesh.m_faces.begin(), mesh.m_faces.end());
	}

	m_elements = (gl::GLsizei)iBufferCombined.size() * 3;

	gl::glNamedBufferStorage(m_vertexBuffer, vBufferCombined.size() * sizeof(glm::vec3), vBufferCombined.data(), gl::GL_NONE_BIT);
	gl::glNamedBufferStorage(m_indexBuffer, iBufferCombined.size() * sizeof(glm::u16vec3), iBufferCombined.data(), gl::GL_NONE_BIT);

	gl::glGenVertexArrays(1, &m_VAO);
	gl::glBindVertexArray(m_VAO);

	gl::glEnableVertexAttribArray(Model::ATTRIB_POS);
	gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, m_vertexBuffer);
	gl::glVertexAttribPointer(Model::ATTRIB_POS, 3, gl::GL_FLOAT, gl::GL_FALSE, 0, nullptr);
	gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
	gl::glBindVertexArray(0);

	gl::GLuint vertShader, fragShader;

	vertShader = loadCompileShader(gl::GL_VERTEX_SHADER, "vert_mvp.vert");
	fragShader = loadCompileShader(gl::GL_FRAGMENT_SHADER, "color.frag");

	m_program = createProgram({ vertShader, fragShader });

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);

	//2x bigger
	m_model = glm::scale(glm::mat4(1.f), glm::vec3(2.0f));
}

Model::~Model()
{
	gl::glDeleteBuffers(1, &m_vertexBuffer);
	gl::glDeleteBuffers(1, &m_indexBuffer);
	gl::glDeleteProgram(m_program);
}

void Model::draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP)
{
	glm::vec3 col(0.5f, 0.5f, 0.5f);
	glm::mat4 MVP = VP * m_model;

	gl::glUseProgram(m_program);

	gl::glUniformMatrix4fv(0, 1, gl::GL_FALSE, value_ptr(MVP));
	gl::glUniform3fv(1, 1, value_ptr(col));

	gl::glBindVertexArray(m_VAO);

	gl::glDrawElements(gl::GL_TRIANGLES, m_elements, gl::GL_UNSIGNED_SHORT, 0);

	//gl::glMultiDrawElements(gl::GL_TRIANGLES, m_drawSizes.data(), gl::GL_UNSIGNED_INT, (const void* const*)m_indexOffsets.data(), (gl::GLsizei)m_drawSizes.size());

	//for(size_t i = 0; i < m_modelMgr->m_meshes.size(); ++i)
	//{
	//	gl::glDrawElements(gl::GL_TRIANGLES, m_drawSizes[i], gl::GL_UNSIGNED_INT, (const void*)m_indexOffsets[i]);
	//}
}
