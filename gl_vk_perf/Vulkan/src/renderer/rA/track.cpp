#include "track.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../common/lineutil.hpp"
#include "../../linepipeline.hpp"

Track::Track(Render::CalcType cType, Render::LineType lType, Render::RecordType rType, const std::vector<glm::vec3> &pointList, glm::vec3 color) : m_engine(Engine::getInstance()), m_cType(cType), m_lType(lType), m_rType(rType)
{
	std::vector<glm::vec3> vertexData;
	std::vector<glm::u16> indexData;

	LineUtil::fillBuffers(cType, lType, pointList, vertexData, indexData);

	m_count = (glm::u16)indexData.size();

	m_engine.createBufferData(m_indexBufferMem, m_indexBuffer, m_count * sizeof(glm::u16), vk::BufferUsageFlagBits::eIndexBuffer, indexData.data());
	m_engine.createBufferData(m_vertexBufferMem, m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, vertexData.data());
	
	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));
	m_color = color;
}

void Track::drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout)
{
	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(m_color));
	cmd.drawIndexed(uint32_t(m_count), 1, 0, 0, 0);
}

void Track::drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(m_color));
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eTessellationEvaluation, sizeof(LinePCColor), sizeof(LinePCMVP), value_ptr(MVP));
	cmd.drawIndexed(uint32_t(m_count), 1, 0, 0, 0);
}

