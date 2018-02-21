#ifndef __H_MODEL__
#define __H_MODEL__

#include <glm/glm.hpp>
#include "engine.hpp"
#include "../common/alimodelmanager.hpp"

class Model
{
	private:
		Engine & m_engine;

		std::unique_ptr<AliModelManager> m_modelMgr;

		vk::UniqueDeviceMemory m_indexBufferMem;
		vk::UniqueDeviceMemory m_vertexBufferMem;
		vk::UniqueBuffer m_vertexBuffer;
		vk::UniqueBuffer m_indexBuffer;

		std::vector<size_t> m_indexOffsets;
		std::vector<uint32_t> m_drawSizes;
		uint32_t m_count;

		glm::mat4 m_model;

	public:
		Model();

		void draw(vk::CommandBuffer cmd, vk::PipelineLayout layout, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);

};

#endif