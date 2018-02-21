#ifndef __H_PIPELINE__
#define __H_PIPELINE__

#include "engine.hpp"

#include <unordered_map>

namespace std
{
	template <>
	struct hash<vk::ShaderStageFlags>
	{
		size_t operator()(const vk::ShaderStageFlags& k) const
		{
			return hash<uint32_t>()(uint32_t(k));
		}
	};
}

class Pipeline
{
	protected:
		Engine & m_engine;

	private:
		const size_t VTX_FRG = 2;
		const size_t VTX_CTL_EVL_FRG = 4;

		vk::UniqueDescriptorSetLayout m_descrSetLayout;
		vk::UniquePipelineLayout m_pipelineLayout;
		vk::UniquePipeline m_pipeline;

		bool m_initialized;
		bool m_swapchainDepInitialized;

		virtual std::vector<vk::DescriptorSetLayoutBinding> getSetLayoutBindings() const;
		virtual std::vector<vk::PushConstantRange> getConstantRanges() const;
		virtual std::vector<vk::VertexInputBindingDescription> getInputBindingDescriptions() const = 0;
		virtual std::vector<vk::VertexInputAttributeDescription> getInputAttributeDescriptions() const = 0;
		virtual vk::PrimitiveTopology getTopology() const = 0;
		virtual vk::PipelineRasterizationStateCreateInfo getRasterizationInfo() const = 0;
		virtual uint32_t getTesselationPatchSize() const = 0;
		virtual bool enableDepthTest() const;

	public:
		Pipeline();
		void init();

		void initSwapchainDependent(vk::RenderPass pass, vk::Rect2D renderArea,  const std::vector<vk::ShaderModule> &shaderModules, const std::unordered_map<vk::ShaderStageFlags, vk::SpecializationInfo> &spMap, vk::Pipeline deriv = vk::Pipeline());
		void freeSwapchainDependent();

		static std::vector<char> readFile(const std::string& filename);
		static vk::UniqueShaderModule createShaderModule(const std::string& filename);

		const std::vector<vk::DescriptorSetLayout> getDescriptorSetLayouts() const;
		const vk::PipelineLayout getPipelineLayout() const;
		const vk::Pipeline getPipeline() const;
};

#endif