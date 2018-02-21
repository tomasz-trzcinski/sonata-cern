#ifndef __H_TRACKARRAYC__
#define __H_TRACKARRAYC__

#include "../common/enums.hpp"
#include "../../linepipeline.hpp"
#include "../common/alieventmanager.hpp"

#include <unordered_map>

class TrackArrayC
{
	private:
		Engine & m_engine;
		Render::CalcType m_cType;
		Render::LineType m_lType;
		Render::RecordType m_rType;

		struct TrackInfo
		{
			uint32_t indexOffset;
			uint32_t vertexOffset;
			uint32_t count;
		};

		struct ParticleList
		{
			glm::vec3 color;
			std::vector<TrackInfo> trackInfos;
		};

		std::unordered_map<int32_t, ParticleList> m_renderList;

		std::unique_ptr<AliEventManager> m_eventMgr;

		vk::UniqueDeviceMemory m_indexBufferMem;
		vk::UniqueDeviceMemory m_vertexBufferMem;
		vk::UniqueBuffer m_vertexBuffer;
		vk::UniqueBuffer m_indexBuffer;
		vk::UniqueDeviceMemory m_uniformBufferMem;
		vk::UniqueBuffer m_uniformBuffer;

		vk::UniqueDescriptorPool m_descriptorPool;
		vk::DescriptorSet m_descriptorSet;

		vk::Pipeline m_pipeline;
		vk::PipelineLayout m_pipelineLayout;

		unsigned int m_cores;
		std::vector<vk::UniqueCommandBuffer> m_commandBuffers;
		std::vector<vk::CommandBuffer> m_cmdReferences;

		glm::u16 m_count;

		glm::mat4 m_model;

		void fillSecondaryDynamic(vk::CommandBufferInheritanceInfo inhInfo, vk::CommandBuffer cmd, glm::mat4 &VP, const ParticleList &list);
		void fillSecondaryStatic(vk::CommandBufferInheritanceInfo inhInfo, vk::CommandBuffer cmd, const ParticleList &list);

	public:
		TrackArrayC(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType, unsigned int cores);

		void drawDynamic(vk::CommandBuffer cmd, vk::RenderPass pass, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
		void prepareStatic(vk::RenderPass pass);
		void drawStatic(vk::CommandBuffer cmd);

		void prevEvent();

		void nextEvent();

		void loadTracks();

		void updateStatic(glm::mat4 &VP);
		void initSwapchainDependent(LinePipeline &pipeline);
};

#endif