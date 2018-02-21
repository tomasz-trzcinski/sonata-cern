#include "linepipeline.hpp"

std::vector<vk::DescriptorSetLayoutBinding> LinePipeline::getSetLayoutBindings() const
{
	vk::DescriptorSetLayoutBinding mvpLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eTessellationEvaluation, nullptr };
	vk::DescriptorSetLayoutBinding colLayoutBinding{ 1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr };
	
	if(color_enabled)
		return { mvpLayoutBinding, colLayoutBinding };
	else
		return { mvpLayoutBinding };
}

std::vector<vk::PushConstantRange> LinePipeline::getConstantRanges() const
{
	vk::PushConstantRange range1{ vk::ShaderStageFlagBits::eVertex, 0, sizeof(LinePCColor) };
	vk::PushConstantRange range2{ vk::ShaderStageFlagBits::eTessellationEvaluation, sizeof(LinePCColor), sizeof(LinePCMVP) };

	return { range1, range2 };
}

std::vector<vk::VertexInputBindingDescription> LinePipeline::getInputBindingDescriptions() const
{
	return LineVertex::getInputBindingDescriptions();
}

std::vector<vk::VertexInputAttributeDescription> LinePipeline::getInputAttributeDescriptions() const
{
	return LineVertex::getInputAttributeDescriptions();
}

vk::PrimitiveTopology LinePipeline::getTopology() const
{
	return vk::PrimitiveTopology::ePatchList;
}

vk::PipelineRasterizationStateCreateInfo LinePipeline::getRasterizationInfo() const
{
	vk::PipelineRasterizationStateCreateInfo prsci;
	prsci.cullMode = vk::CullModeFlagBits::eNone;
	prsci.frontFace = vk::FrontFace::eCounterClockwise;
	prsci.lineWidth = 1.0f;

	return prsci;
}

uint32_t LinePipeline::getTesselationPatchSize() const
{
	return 4;
}

bool LinePipeline::enableDepthTest() const
{
	return true;
}