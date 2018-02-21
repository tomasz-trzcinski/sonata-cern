#ifndef __H_TRACKARRAYD__
#define __H_TRACKARRAYD__

#include "../common/enums.hpp"
#include "../../linepipeline.hpp"
#include "../common/alieventmanager.hpp"

#include <unordered_map>

class TrackArrayD
{
	private:
		Engine & m_engine;
		Render::CalcType m_cType;
		Render::LineType m_lType;
		Render::RecordType m_rType;

		std::unordered_map<int32_t, glm::vec3> m_renderList;

		std::unique_ptr<AliEventManager> m_eventMgr;

		vk::UniqueDeviceMemory m_indexBufferMem;
		vk::UniqueDeviceMemory m_vertexBufferMem;
		vk::UniqueBuffer m_vertexBuffer;
		vk::UniqueBuffer m_indexBuffer;
		vk::UniqueDeviceMemory m_uniformBufferMem;
		vk::UniqueBuffer m_uniformBuffer;
		vk::UniqueDeviceMemory m_colorBufferMem;
		vk::UniqueBuffer m_colorBuffer;
		vk::UniqueDeviceMemory m_indirectBufferMem;
		vk::UniqueBuffer m_indirectBuffer;

		vk::UniqueDescriptorPool m_descriptorPool;
		vk::DescriptorSet m_descriptorSet;
		vk::PipelineLayout m_layout;

		uint32_t m_count;

		glm::mat4 m_model;

	public:
		TrackArrayD(LinePipeline &pipeline, Render::CalcType cType, Render::LineType lType, Render::RecordType rType);

		void drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
		void drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout);

		void prevEvent();

		void nextEvent();

		void loadTracks();

		void updateStatic(glm::mat4 &VP);
};

#endif