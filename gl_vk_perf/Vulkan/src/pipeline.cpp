#include "pipeline.hpp"

#include <fstream>
#include <iostream>

Pipeline::Pipeline() : m_engine(Engine::getInstance()), m_initialized(false), m_swapchainDepInitialized(false)
{
}

void Pipeline::init()
{
	const auto descriptorBindings = getSetLayoutBindings();
	vk::DescriptorSetLayoutCreateInfo dslci{ {}, uint32_t(descriptorBindings.size()), descriptorBindings.data() };
	m_descrSetLayout = m_engine.getDevice().createDescriptorSetLayoutUnique(dslci);

	const auto constantRanges = getConstantRanges();

	const std::vector<vk::DescriptorSetLayout> setLayouts = { m_descrSetLayout.get() };

	vk::PipelineLayoutCreateInfo plci{ {},
		uint32_t(setLayouts.size()), setLayouts.data(),
		uint32_t(constantRanges.size()), constantRanges.data()
	};
	m_pipelineLayout = m_engine.getDevice().createPipelineLayoutUnique(plci);

	m_initialized = true;
}

void Pipeline::initSwapchainDependent(vk::RenderPass pass, vk::Rect2D renderArea, const std::vector<vk::ShaderModule> &shaderModules, const std::unordered_map<vk::ShaderStageFlags, vk::SpecializationInfo> &spMap, vk::Pipeline deriv)
{
	if (!m_initialized)
		throw std::runtime_error("Pipeline swapchain independent not initialized!");

	std::vector<vk::ShaderStageFlagBits> moduleFlags;
	if (shaderModules.size() == VTX_FRG)
	{
		moduleFlags.push_back(vk::ShaderStageFlagBits::eVertex);
		moduleFlags.push_back(vk::ShaderStageFlagBits::eFragment);
	}
	else if (shaderModules.size() == VTX_CTL_EVL_FRG)
	{
		moduleFlags.push_back(vk::ShaderStageFlagBits::eVertex);
		moduleFlags.push_back(vk::ShaderStageFlagBits::eTessellationControl);
		moduleFlags.push_back(vk::ShaderStageFlagBits::eTessellationEvaluation);
		moduleFlags.push_back(vk::ShaderStageFlagBits::eFragment);
	}
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	for (size_t i = 0; i < shaderModules.size(); ++i)
	{
		const vk::SpecializationInfo *spPointer = nullptr;
		auto const element = spMap.find(moduleFlags[i]);
		if (element != spMap.end())
		{
			spPointer = &element->second;
		}
		vk::PipelineShaderStageCreateInfo pssci{ {}, moduleFlags[i], shaderModules[i], "main", spPointer };
		shaderStages.push_back(pssci);
	}
	const auto inputBindingDescriptions = getInputBindingDescriptions();
	const auto inputAttributeDescriptions = getInputAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo pvisci{ {},
		uint32_t(inputBindingDescriptions.size()), inputBindingDescriptions.data(),
		uint32_t(inputAttributeDescriptions.size()), inputAttributeDescriptions.data()
	};



	vk::PipelineInputAssemblyStateCreateInfo piasci{ {}, getTopology(), VK_FALSE };



	std::vector<vk::Viewport> viewPorts{
		{
			float(renderArea.offset.x), float(renderArea.offset.y),
			float(renderArea.extent.width), float(renderArea.extent.height),
			0.0f, 1.0f
		}
	};
	std::vector<vk::Rect2D> scissors{ renderArea };
	vk::PipelineViewportStateCreateInfo pvsci{ {}, uint32_t(viewPorts.size()), viewPorts.data(), uint32_t(scissors.size()), scissors.data() };



	vk::PipelineRasterizationStateCreateInfo prsci = getRasterizationInfo();



	vk::PipelineMultisampleStateCreateInfo pmsci{};



	bool depthTest = enableDepthTest();
	bool depthWrite = enableDepthTest();

	vk::PipelineDepthStencilStateCreateInfo pdssci{ {}, depthTest, depthWrite, vk::CompareOp::eLess, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f };



	vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = ~vk::ColorComponentFlagBits(); //all flags
	std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachements{ colorBlendAttachment };
	vk::PipelineColorBlendStateCreateInfo pcbsci{};
	pcbsci.attachmentCount = uint32_t(colorBlendAttachements.size());
	pcbsci.pAttachments = colorBlendAttachements.data();


	
	vk::PipelineTessellationStateCreateInfo ptsci{ {}, getTesselationPatchSize() };



	vk::GraphicsPipelineCreateInfo gpci{};
	gpci.stageCount = uint32_t(shaderStages.size());
	gpci.pStages = shaderStages.data();
	gpci.pVertexInputState = &pvisci;
	gpci.pInputAssemblyState = &piasci;
	gpci.pTessellationState = &ptsci;
	gpci.pViewportState = &pvsci;
	gpci.pRasterizationState = &prsci;
	gpci.pMultisampleState = &pmsci;
	gpci.pDepthStencilState = &pdssci;
	gpci.pColorBlendState = &pcbsci;
	gpci.pDynamicState = nullptr;
	gpci.layout = m_pipelineLayout.get();
	gpci.renderPass = pass;
	gpci.subpass = 0;
	gpci.basePipelineHandle = deriv;
	gpci.basePipelineIndex = 0;

	m_pipeline = m_engine.getDevice().createGraphicsPipelineUnique(vk::PipelineCache(), gpci);
	m_swapchainDepInitialized = true;
}

void Pipeline::freeSwapchainDependent()
{
	m_pipeline.reset();
	m_swapchainDepInitialized = false;
}

std::vector<vk::DescriptorSetLayoutBinding> Pipeline::getSetLayoutBindings() const
{
	return {};
}

std::vector<vk::PushConstantRange> Pipeline::getConstantRanges() const
{
	return {};
}

bool Pipeline::enableDepthTest() const
{
	return false;
}

std::vector<char> Pipeline::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

vk::UniqueShaderModule Pipeline::createShaderModule(const std::string& filename)
{
	std::vector<char> code = readFile("shaders/" + filename);

	vk::ShaderModuleCreateInfo moduleInfo{};
	moduleInfo.codeSize = code.size();
	moduleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	return Engine::getInstance().getDevice().createShaderModuleUnique(moduleInfo);
}

const std::vector<vk::DescriptorSetLayout> Pipeline::getDescriptorSetLayouts() const
{
	if (!m_initialized)
		throw std::runtime_error("Pipeline swapchain independent not initialized!");

	return { m_descrSetLayout.get() };
}

const vk::PipelineLayout Pipeline::getPipelineLayout() const
{
	if (!m_initialized)
		throw std::runtime_error("Pipeline swapchain dependent not initialized!");

	return m_pipelineLayout.get();
}

const vk::Pipeline Pipeline::getPipeline() const
{
	if(!m_swapchainDepInitialized)
		throw std::runtime_error("Pipeline swapchain dependent not initialized!");

	return m_pipeline.get();
}
