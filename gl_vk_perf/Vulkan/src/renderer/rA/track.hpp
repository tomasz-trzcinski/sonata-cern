#ifndef __H_TRACK__
#define __H_TRACK__

#include "../renderer.hpp"
#include "../common/enums.hpp"

class Track
{
	private:
		Engine & m_engine;
		Render::CalcType m_cType;
		Render::LineType m_lType;
		Render::RecordType m_rType;

		vk::UniqueDeviceMemory m_indexBufferMem;
		vk::UniqueDeviceMemory m_vertexBufferMem;
		vk::UniqueBuffer m_vertexBuffer;
		vk::UniqueBuffer m_indexBuffer;

		glm::u16 m_count;

		glm::mat4 m_model;
		glm::vec3 m_color;

	public:
		Track(Render::CalcType cType, Render::LineType lType, Render::RecordType rType, const std::vector<glm::vec3> &pointList, glm::vec3 color);

		void drawStatic(vk::CommandBuffer cmd, vk::PipelineLayout layout);
		void drawDynamic(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
};

#endif