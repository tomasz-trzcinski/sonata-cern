#ifndef __H_MODELPIPELINE__
#define __H_MODELPIPELINE__

#include <glm/glm.hpp>

#include "pipeline.hpp"

struct ModelVertex
{
	glm::vec3 pos;

	static std::vector<vk::VertexInputBindingDescription> getInputBindingDescriptions()
	{
		vk::VertexInputBindingDescription bindingDescription{ 0, sizeof(ModelVertex), vk::VertexInputRate::eVertex };

		return { bindingDescription };
	}

	static std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions()
	{
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

		vk::VertexInputAttributeDescription positionDescription{ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(ModelVertex, pos) };
		//vk::VertexInputAttributeDescription colorDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(LineVertex, color) };

		attributeDescriptions.push_back(positionDescription);

		return attributeDescriptions;
	}
};

struct ModelUBlock
{
	glm::mat4 MVP;
};

struct ModelPCBlock
{
	glm::mat4 MVP;
	glm::vec3 color;
};

class ModelPipeline : public Pipeline
{
public:
	ModelPipeline() {}
private:
	//virtual std::vector<vk::DescriptorSetLayoutBinding> getSetLayoutBindings() const;
	virtual std::vector<vk::PushConstantRange> getConstantRanges() const;
	virtual std::vector<vk::VertexInputBindingDescription> getInputBindingDescriptions() const;
	virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const;
	virtual vk::PrimitiveTopology getTopology() const;
	virtual vk::PipelineRasterizationStateCreateInfo getRasterizationInfo() const;
	virtual uint32_t getTesselationPatchSize() const;
	bool enableDepthTest() const;
};

#endif