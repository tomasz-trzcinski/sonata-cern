#include "model.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "modelpipeline.hpp"

Model::Model() : m_engine(Engine::getInstance())
{
	m_modelMgr = std::make_unique<AliModelManager>("alicegeom.obj");

	std::vector<glm::vec3> vBufferCombined;
	std::vector<glm::u16vec3> iBufferCombined;

	for (const AliModelManager::Mesh &mesh : m_modelMgr->m_meshes)
	{
		vBufferCombined.insert(vBufferCombined.end(), mesh.m_vertices.begin(), mesh.m_vertices.end());
		m_indexOffsets.push_back(iBufferCombined.size());
		m_drawSizes.push_back(uint32_t(mesh.m_faces.size() * 3));
		//Convert from unsigned ints to shorts, takes half as much memory
		iBufferCombined.insert(iBufferCombined.end(), mesh.m_faces.begin(), mesh.m_faces.end());
	}

	m_count = uint32_t(iBufferCombined.size() * 3);

	m_engine.createBufferData(m_indexBufferMem, m_indexBuffer, iBufferCombined.size() * sizeof(glm::u16vec3), vk::BufferUsageFlagBits::eIndexBuffer, iBufferCombined.data());
	m_engine.createBufferData(m_vertexBufferMem, m_vertexBuffer, vBufferCombined.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, vBufferCombined.data());

	m_model = glm::scale(glm::mat4(1.f), glm::vec3(1.0f));
}

void Model::draw(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	const ModelPCBlock block{ VP * m_model, glm::vec3(0.3f, 0.2f, 0.3f) };

	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ModelPCBlock), &block);

	cmd.drawIndexed(m_count, 1, 0, 0, 0);
}