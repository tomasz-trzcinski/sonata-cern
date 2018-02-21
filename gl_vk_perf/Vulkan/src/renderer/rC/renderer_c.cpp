#include "renderer_c.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <iostream>
#include <thread>

RendererC::RendererC(const Swapchain &swapchain, Render::CalcType cType, Render::LineType lType, Render::RecordType rType) : m_cType(cType), m_lType(lType), m_rType(rType)
{
	m_lineVertexShader = Pipeline::createShaderModule("uniform_col.vert.spv");
	m_lineControlShader = Pipeline::createShaderModule("bezier.tesc.spv");
	m_lineEvaluationShaderPush = Pipeline::createShaderModule("bezier_push.tese.spv");
	m_lineEvaluationShaderUniform = Pipeline::createShaderModule("bezier_uniform.tese.spv");
	m_lineFragmentShader = Pipeline::createShaderModule("color.frag.spv");

	m_modelVertexShader = Pipeline::createShaderModule("vert_mvp.vert.spv");
	m_modelFragmentShader = Pipeline::createShaderModule("color.frag.spv");

	m_linePipeline = std::make_unique<LinePipeline>();
	m_linePipeline->init();

	m_modelPipeline = std::make_unique<ModelPipeline>();
	m_modelPipeline->init();

	m_tArray = std::make_unique<TrackArrayC>(*m_linePipeline.get(), cType, lType, rType, std::thread::hardware_concurrency());
	m_model = std::make_unique<Model>();

	initSwapchainDependent(swapchain);
}

RendererC::~RendererC()
{
}

vk::CommandBuffer RendererC::draw(unsigned long duration, uint32_t imageIndex)
{
	float angle = (duration / 15000.0f) * 2 * glm::pi<float>();

	float radius = 20.f;

	glm::vec3 cameraPos(radius * glm::sin(angle), radius, radius * glm::cos(angle));

	m_view = glm::lookAt(cameraPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	glm::mat4 VP = m_projection * m_view;

	if (m_rType == Render::RecordType::STATIC)
	{
		m_tArray->updateStatic(VP);
		vk::CommandBuffer cmd = m_commandBuffers[imageIndex].get();
		return cmd;
	}
	else
	{
		return drawDynamic(imageIndex, duration, cameraPos, VP);
	}
}

void RendererC::drawStatic(uint32_t imageIndex)
{
	std::array<vk::ClearValue, 2> clearValues{};
	clearValues[0] = vk::ClearColorValue(std::array<float, 4>({ 0.1f, 0.1f, 0.2f, 1.0f }));
	clearValues[1] = vk::ClearDepthStencilValue(1.0f, 0);

	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eSimultaneousUse };
	vk::RenderPassBeginInfo renderPassInfo{ m_renderPass->getPass(), m_renderPass->getFramebuffer(imageIndex), m_renderArea, uint32_t(clearValues.size()), clearValues.data() };

	vk::CommandBuffer cmd = m_commandBuffers[imageIndex].get();

	cmd.begin(beginInfo);
	cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);

	//cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_modelPipeline->getPipeline());

	//m_model->draw(cmd, m_modelPipeline->getPipelineLayout(), duration, cameraPos, VP);

	m_tArray->drawStatic(cmd);

	cmd.endRenderPass();
	cmd.end();
}

vk::CommandBuffer RendererC::drawDynamic(uint32_t imageIndex, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP)
{
	std::array<vk::ClearValue, 2> clearValues{};
	clearValues[0] = vk::ClearColorValue(std::array<float, 4>({ 0.1f, 0.1f, 0.2f, 1.0f }));
	clearValues[1] = vk::ClearDepthStencilValue(1.0f, 0);

	vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eSimultaneousUse };
	vk::RenderPassBeginInfo renderPassInfo{ m_renderPass->getPass(), m_renderPass->getFramebuffer(imageIndex), m_renderArea, uint32_t(clearValues.size()), clearValues.data() };

	vk::CommandBuffer cmd = m_commandBuffers[imageIndex].get();

	cmd.begin(beginInfo);
	cmd.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);

	//cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_modelPipeline->getPipeline());

	//m_model->draw(cmd, m_modelPipeline->getPipelineLayout(), duration, cameraPos, VP);

	m_tArray->drawDynamic(cmd, m_renderPass->getPass(), duration, cameraPos, VP);

	cmd.endRenderPass();
	cmd.end();

	return cmd;
}

void RendererC::prevEvent()
{
	m_tArray->prevEvent();
}

void RendererC::nextEvent()
{
	m_tArray->nextEvent();
}

void RendererC::initSwapchainDependent(const Swapchain &swapchain)
{
	Renderer::initSwapchainDependent(swapchain);

	m_renderPass = std::make_unique<Renderpass>(swapchain);

	std::vector<vk::ShaderModule> lineModules;
	
	if (m_rType == Render::RecordType::DYNAMIC)
	{
		lineModules.push_back(m_lineVertexShader.get());
		lineModules.push_back(m_lineControlShader.get());
		lineModules.push_back(m_lineEvaluationShaderPush.get());
		lineModules.push_back(m_lineFragmentShader.get());
	}
	else
	{
		lineModules.push_back(m_lineVertexShader.get());
		lineModules.push_back(m_lineControlShader.get());
		lineModules.push_back(m_lineEvaluationShaderUniform.get());
		lineModules.push_back(m_lineFragmentShader.get());
	}
	
	int LINE_MODEL;

	if (m_cType == Render::CalcType::CPU)
		LINE_MODEL = 0; //select passthrough assembly
	else if (m_lType == Render::LineType::GEOM)
		LINE_MODEL = 1; //select geometric assembly
	else if (m_lType == Render::LineType::HOBBY)
		LINE_MODEL = 2; //select hobby assembly

	vk::SpecializationMapEntry entry{ 0, 0, sizeof(int) };
	vk::SpecializationInfo sInfo{ 1, &entry, sizeof(int), &LINE_MODEL };

	std::unordered_map<vk::ShaderStageFlags, vk::SpecializationInfo> shaderMap{ {vk::ShaderStageFlagBits::eTessellationControl, sInfo} };

	m_linePipeline->initSwapchainDependent(m_renderPass->getPass(), swapchain.getRenderArea(), lineModules, shaderMap);
	
	std::vector<vk::ShaderModule> modelModules{ m_modelVertexShader.get(), m_modelFragmentShader.get() };
	m_modelPipeline->initSwapchainDependent(m_renderPass->getPass(), swapchain.getRenderArea(), modelModules, {}, m_linePipeline->getPipeline());

	vk::CommandBufferAllocateInfo allocInfo{ m_engine.getWorkCommandPool(), vk::CommandBufferLevel::ePrimary, uint32_t(swapchain.getNumImages()) };
	m_commandBuffers = m_engine.getDevice().allocateCommandBuffersUnique(allocInfo);

	m_tArray->initSwapchainDependent(*m_linePipeline.get());

	if (m_rType == Render::RecordType::STATIC)
	{
		m_tArray->prepareStatic(m_renderPass->getPass());
		for (uint32_t i = 0; i < swapchain.getNumImages(); ++i)
		{
			drawStatic(i);
		}
	}
}

void RendererC::freeSwapchainDependent()
{
	m_commandBuffers.clear();
	m_linePipeline->freeSwapchainDependent();
	m_modelPipeline->freeSwapchainDependent();
	m_renderPass.reset();
}
