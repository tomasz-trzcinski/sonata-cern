#include "track_array_d.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "../common/lineutil.hpp"
#include "../../linepipeline.hpp"

#include <iostream>

TrackArrayD::TrackArrayD(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType) : m_engine(Engine::getInstance()), m_cType(cType), m_lType(lType), m_rType(rType)
{
	m_eventMgr = std::make_unique<AliEventManager>();

	m_engine.createBuffer(m_uniformBufferMem, m_uniformBuffer, sizeof(LineUBlock), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	std::vector<vk::DescriptorPoolSize> poolSize{ {vk::DescriptorType::eUniformBuffer, 1}, { vk::DescriptorType::eStorageBuffer, 1 } };
	vk::DescriptorPoolCreateInfo poolInfo{ {}, 1, uint32_t(poolSize.size()), poolSize.data() };
	m_descriptorPool = m_engine.getDevice().createDescriptorPoolUnique(poolInfo);

	vk::DescriptorSetLayout layout = pipeline.getDescriptorSetLayouts()[0];
	vk::DescriptorSetAllocateInfo allocSetInfo{ m_descriptorPool.get(), 1, &layout };
	std::vector<vk::DescriptorSet> descriptorSets = m_engine.getDevice().allocateDescriptorSets(allocSetInfo);
	m_descriptorSet = descriptorSets[0];

	if (rType == Render::RecordType::STATIC)
	{
		vk::DescriptorBufferInfo bufferInfo{ m_uniformBuffer.get(), 0, sizeof(LineUBlock) };
		vk::WriteDescriptorSet descriptorWrite{ m_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo, nullptr };
		m_engine.getDevice().updateDescriptorSets({ descriptorWrite }, {});
	}

	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));

	loadTracks();
}

void TrackArrayD::drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, { m_descriptorSet }, {});
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eTessellationEvaluation, sizeof(LinePCColor), sizeof(LinePCMVP), value_ptr(MVP));

	cmd.drawIndexedIndirect(m_indirectBuffer.get(), vk::DeviceSize{ 0 }, m_count, sizeof(vk::DrawIndexedIndirectCommand));
}

void TrackArrayD::drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout)
{
	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, { m_descriptorSet }, {});

	cmd.drawIndexedIndirect(m_indirectBuffer.get(), vk::DeviceSize{ 0 }, m_count, sizeof(vk::DrawIndexedIndirectCommand));
}

void TrackArrayD::prevEvent()
{
	m_eventMgr->prevEvent();

	loadTracks();
}

void TrackArrayD::nextEvent()
{
	m_eventMgr->nextEvent();

	loadTracks();
}

void TrackArrayD::loadTracks()
{
	m_renderList.clear();

	std::vector<glm::vec3> vertexData;
	std::vector<glm::u16> indexData;
	std::vector<glm::vec3> colorData;
	std::vector<vk::DrawIndexedIndirectCommand> indirectData;

	for (auto const &track : m_eventMgr->m_tracks)
	{
		if (track.m_pointList.size() > 2)
		{
			size_t vertexOffset = vertexData.size();
			size_t indexStart = indexData.size();

			LineUtil::fillBuffers(m_cType, m_lType, track.m_pointList, vertexData, indexData);

			size_t count = indexData.size() - indexStart;

			indirectData.push_back(vk::DrawIndexedIndirectCommand(uint32_t(count), 1, uint32_t(indexStart), uint32_t(vertexOffset), 0));

			auto key = m_renderList.find(track.m_PID);

			if (key == m_renderList.end())
			{
				glm::vec3 color = glm::sphericalRand(1.0f);
				m_renderList.emplace(std::pair<int32_t, glm::vec3>(track.m_PID, color));
				colorData.push_back(color);
			}
			else
			{
				colorData.push_back(key->second);
			}
		}
	}

	m_engine.getDevice().waitIdle();

	m_engine.createBufferData(m_indexBufferMem, m_indexBuffer, indexData.size() * sizeof(glm::u16), vk::BufferUsageFlagBits::eIndexBuffer, indexData.data());
	m_engine.createBufferData(m_vertexBufferMem, m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, vertexData.data());
	m_engine.createBufferData(m_colorBufferMem, m_colorBuffer, colorData.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eStorageBuffer, colorData.data());
	m_engine.createBufferData(m_indirectBufferMem, m_indirectBuffer, indirectData.size() * sizeof(vk::DrawIndexedIndirectCommand), vk::BufferUsageFlagBits::eIndirectBuffer, indirectData.data());

	vk::DescriptorBufferInfo bufferInfo{ m_colorBuffer.get(), 0, colorData.size() * sizeof(glm::vec3) };
	vk::WriteDescriptorSet descriptorWrite{ m_descriptorSet, 1, 0, 1, vk::DescriptorType::eStorageBuffer, nullptr, &bufferInfo, nullptr };
	m_engine.getDevice().updateDescriptorSets({ descriptorWrite }, {});

	m_count = uint32_t(indirectData.size());
}

void TrackArrayD::updateStatic(glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;
	m_engine.copyMemory(m_uniformBufferMem.get(), sizeof(LineUBlock), value_ptr(MVP), 0);
}
