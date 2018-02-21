#ifndef __H_LINEPIPELINE__
#define __H_LINEPIPELINE__

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "pipeline.hpp"

struct LineVertex
{
	glm::vec3 pos;

	static std::vector<vk::VertexInputBindingDescription> getInputBindingDescriptions()
	{
		vk::VertexInputBindingDescription bindingDescription{ 0, sizeof(LineVertex), vk::VertexInputRate::eVertex };

		return { bindingDescription };
	}

	static std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions()
	{
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;

		vk::VertexInputAttributeDescription positionDescription{ 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(LineVertex, pos) };
		//vk::VertexInputAttributeDescription colorDescription{ 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(LineVertex, color) };

		attributeDescriptions.push_back(positionDescription);

		return attributeDescriptions;
	}
};

struct LineUBlock
{
	glm::mat4 MVP;
};

struct LinePCMVP
{
	glm::mat4 MVP;
};

struct LinePCColor
{
	glm::vec3 color;
	float padding;
};

class LinePipeline : public Pipeline
{
	public:
		LinePipeline(bool color = false): color_enabled(color) {}
	private:
		bool color_enabled;

		virtual std::vector<vk::DescriptorSetLayoutBinding> getSetLayoutBindings() const;
		virtual std::vector<vk::PushConstantRange> getConstantRanges() const;
		virtual std::vector<vk::VertexInputBindingDescription> getInputBindingDescriptions() const;
		virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const;
		virtual vk::PrimitiveTopology getTopology() const;
		virtual vk::PipelineRasterizationStateCreateInfo getRasterizationInfo() const;
		virtual uint32_t getTesselationPatchSize() const;
		bool enableDepthTest() const;
};

#endif