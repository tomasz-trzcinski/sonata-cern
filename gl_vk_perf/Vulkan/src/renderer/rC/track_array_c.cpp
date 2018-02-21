#include "track_array_c.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "../common/lineutil.hpp"
#include "../../linepipeline.hpp"

#include <thread>
#include <iostream>

TrackArrayC::TrackArrayC(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType, unsigned int cores) : m_engine(Engine::getInstance()), m_cType(cType), m_lType(lType), m_rType(rType), m_cores(cores)
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

	m_pipelineLayout = pipeline.getPipelineLayout();

	m_model = glm::scale(glm::mat4(1.f), glm::vec3(0.02f));

	loadTracks();
}

void TrackArrayC::drawDynamic(vk::CommandBuffer cmd, vk::RenderPass pass, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	vk::CommandBufferInheritanceInfo inherInfo{ pass, 0 };

	uint32_t i = 0;
	
	std::vector<std::thread> recorders;
	for (const auto &lElement : m_renderList)
	{
		//recorders.push_back(std::thread(&TrackArrayC::fillSecondaryDynamic, this, inherInfo, m_cmdReferences[i], std::ref(VP), std::ref(lElement.second)));
		fillSecondaryDynamic(inherInfo, m_cmdReferences[i], VP, lElement.second);
		++i;
	}

	//for (auto &thread : recorders)
	//	thread.join();

	cmd.executeCommands(m_cmdReferences);
}

void TrackArrayC::prepareStatic(vk::RenderPass pass)
{
	vk::CommandBufferInheritanceInfo inherInfo{ pass, 0 };

	uint32_t i = 0;

	std::vector<std::thread> recorders;
	for (const auto &lElement : m_renderList)
	{
		recorders.push_back(std::thread(&TrackArrayC::fillSecondaryStatic, this, inherInfo, m_cmdReferences[i], std::ref(lElement.second)));
		//fillSecondaryStatic(inherInfo, m_cmdReferences[i], lElement.second);
		++i;
	}

	for (auto &thread : recorders)
		thread.join();
}

void TrackArrayC::drawStatic(vk::CommandBuffer cmd)
{
	cmd.executeCommands(m_cmdReferences);
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

void TrackArrayC::fillSecondaryDynamic(vk::CommandBufferInheritanceInfo inhInfo, vk::CommandBuffer cmd, glm::mat4 &VP, const ParticleList &list)
{
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inhInfo };

	glm::mat4 MVP = VP * m_model;

	cmd.begin(beginInfo);

	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);

	cmd.pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eTessellationEvaluation, sizeof(LinePCColor), sizeof(LinePCMVP), value_ptr(MVP));

	cmd.pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(list.color));
	for (const auto &particle : list.trackInfos)
	{
		cmd.drawIndexed(particle.count, 1, particle.indexOffset, particle.vertexOffset, 0);
	}

	cmd.end();
}

void TrackArrayC::fillSecondaryStatic(vk::CommandBufferInheritanceInfo inhInfo, vk::CommandBuffer cmd, const ParticleList &list)
{
	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue, &inhInfo };

	cmd.begin(beginInfo);

	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
	cmd.bindVertexBuffers(0, { m_vertexBuffer.get() }, { vk::DeviceSize{ 0 } });
	cmd.bindIndexBuffer(m_indexBuffer.get(), 0, vk::IndexType::eUint16);

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout, 0, { m_descriptorSet }, {});

	cmd.pushConstants(m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor), value_ptr(list.color));
	for (const auto &particle : list.trackInfos)
	{
		cmd.drawIndexed(particle.count, 1, particle.indexOffset, particle.vertexOffset, 0);
	}

	cmd.end();
}

void TrackArrayC::loadTracks()
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

	vk::CommandBufferAllocateInfo allocInfo{ m_engine.getWorkCommandPool(), vk::CommandBufferLevel::eSecondary, uint32_t(m_renderList.size()) };
	m_commandBuffers = m_engine.getDevice().allocateCommandBuffersUnique(allocInfo);

	for (const auto &c : m_commandBuffers)
		m_cmdReferences.push_back(c.get());
}

void TrackArrayC::updateStatic(glm::mat4 &VP)
{
	glm::mat4 MVP = VP * m_model;
	m_engine.copyMemory(m_uniformBufferMem.get(), sizeof(LineUBlock), value_ptr(MVP), 0);
}

void TrackArrayC::initSwapchainDependent(LinePipeline &pipeline)
{
	m_pipeline = pipeline.getPipeline();
}
