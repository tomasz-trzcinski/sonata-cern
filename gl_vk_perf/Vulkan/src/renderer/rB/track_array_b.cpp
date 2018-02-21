#include "track_array_b.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "../common/lineutil.hpp"
#include "../../linepipeline.hpp"

#include <iostream>

TrackArrayB::TrackArrayB(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType) : m_engine(Engine::getInstance()), m_cType(cType), m_lType(lType), m_rType(rType)
{
	m_eventMgr = std::make_unique<AliEventManager>();

	m_engine.createBuffer(m_uniformBufferMem, m_uniformBuffer, sizeof(LineUBlock), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	if (rType == Render::RecordType::STATIC)
	{
		vk::DescriptorPoolSize poolSize{ vk::DescriptorType::eUniformBuffer, 1 };
		vk::DescriptorPoolCreateInfo poolInfo{ {}, 1, 1, &poolSize };
		m_descriptorPool = m_engine.getDevice().createDescriptorPoolUnique(poolInfo);

		vk::DescriptorSetLayout layout = pipeline.getDescriptorSetLayouts()[0];
		vk::DescriptorSetAllocateInfo allocSetInfo{ m_descriptorPool.get(), 1, &layout };
		std::vector<vk::DescriptorSet> descriptorSets = m_engine.getDevice().allocateDescriptorSets(allocSetInfo);
		m_descriptorSet = descriptorSets[0];

		vk::DescriptorBufferInfo bufferInfo{ m_uniformBuffer.get(), 0, sizeof(LineUBlock) };
		vk::WriteDescriptorSet descriptorWrite{ m_descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo, nullptr };
		m_engine.getDevice().updateDescriptorSets({ descriptorWrite }, {});
	}

	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));

	loadTracks();
}

void TrackArrayB::drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;

	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.pushConstants(layout, vk::ShaderStageFlagBits::eTessellationEvaluation, sizeof(LinePCColor), sizeof(LinePCMVP), value_ptr(MVP));

	for (const auto& pList : m_renderList)
	{
		cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(pList.second.color));
		for (const auto &particle : pList.second.trackInfos)
		{
			cmd.drawIndexed(particle.count, 1, particle.indexOffset, particle.vertexOffset, 0);
		}
	}
}

void TrackArrayB::drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout)
{
	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, { m_descriptorSet }, {});

	for (const auto& pList : m_renderList)
	{
		cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(pList.second.color));
		for (const auto &particle : pList.second.trackInfos)
		{
			cmd.drawIndexed(particle.count, 1, particle.indexOffset, particle.vertexOffset, 0);
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
	m_renderList.clear();

	std::vector<glm::vec3> vertexData;
	std::vector<glm::u16> indexData;

	for (auto const &track : m_eventMgr->m_tracks)
	{
		if (track.m_pointList.size() > 2)
		{
			size_t vertexOffset = vertexData.size();
			size_t indexStart = indexData.size();

			LineUtil::fillBuffers(m_cType, m_lType, track.m_pointList, vertexData, indexData);

			size_t count = indexData.size() - indexStart;

			auto info = m_renderList.find(track.m_PID);

			TrackInfo tInfo{ uint32_t(indexStart), uint32_t(vertexOffset), uint32_t(count) };

			if (info == m_renderList.end())
			{
				m_renderList.emplace(std::pair<int32_t, ParticleList>(track.m_PID, { glm::sphericalRand(1.0f),{ tInfo } }));
			}
			else
			{
				info->second.trackInfos.push_back(tInfo);
			}
		}
	}

	m_engine.getDevice().waitIdle();

	m_engine.createBufferData(m_indexBufferMem, m_indexBuffer, indexData.size() * sizeof(glm::u16), vk::BufferUsageFlagBits::eIndexBuffer, indexData.data());
	m_engine.createBufferData(m_vertexBufferMem, m_vertexBuffer, vertexData.size() * sizeof(glm::vec3), vk::BufferUsageFlagBits::eVertexBuffer, vertexData.data());
}

void TrackArrayB::updateStatic(glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;
	m_engine.copyMemory(m_uniformBufferMem.get(), sizeof(LineUBlock), value_ptr(MVP), 0);
}
