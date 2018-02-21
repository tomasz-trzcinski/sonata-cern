#include "modelpipeline.hpp"

/*
std::vector<vk::DescriptorSetLayoutBinding> ModelPipeline::getSetLayoutBindings() const
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr };

	return { uboLayoutBinding };
}
*/

std::vector<vk::PushConstantRange> ModelPipeline::getConstantRanges() const
{
	vk::PushConstantRange range{ vk::ShaderStageFlagBits::eVertex, 0, sizeof(ModelPCBlock) };

	return { range };
}

std::vector<vk::VertexInputBindingDescription> ModelPipeline::getInputBindingDescriptions() const
{
	return ModelVertex::getInputBindingDescriptions();
}

std::vector<vk::VertexInputAttributeDescription> ModelPipeline::getInputAttributeDescriptions() const
{
	return ModelVertex::getInputAttributeDescriptions();
}

vk::PrimitiveTopology ModelPipeline::getTopology() const
{
	return vk::PrimitiveTopology::eTriangleList;
}

vk::PipelineRasterizationStateCreateInfo ModelPipeline::getRasterizationInfo() const
{
	vk::PipelineRasterizationStateCreateInfo prsci;
	prsci.cullMode = vk::CullModeFlagBits::eBack;
	prsci.frontFace = vk::FrontFace::eCounterClockwise;
	prsci.lineWidth = 1.0f;

	return prsci;
}

uint32_t ModelPipeline::getTesselationPatchSize() const
{
	return 0;
}

bool ModelPipeline::enableDepthTest() const
{
	return true;
}
