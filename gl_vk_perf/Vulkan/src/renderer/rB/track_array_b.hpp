#ifndef __H_TRACKARRAYB__
#define __H_TRACKARRAYB__

#include "../common/enums.hpp"
#include "../../linepipeline.hpp"
#include "../common/alieventmanager.hpp"

#include <unordered_map>

class TrackArrayB
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

		glm::u16 m_count;

		glm::mat4 m_model;

	public:
		TrackArrayB(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType);

		void drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
		void drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout);

		void prevEvent();

		void nextEvent();

		void loadTracks();

		void updateStatic(glm::mat4 &VP);
};

#endif