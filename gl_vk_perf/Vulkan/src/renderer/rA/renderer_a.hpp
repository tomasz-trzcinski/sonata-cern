#ifndef __H_RENDERERA__
#define __H_RENDERERA__

#include <memory>
#include <vector>

#include "../renderer.hpp"
#include "../../swapchain.hpp"
#include "../../renderpass.hpp"

#include "../common/alieventmanager.hpp"

#include "../../linepipeline.hpp"
#include "track.hpp"

#include "../../modelpipeline.hpp"
#include "../../model.hpp"

class RendererA : public Renderer
{
	private:
		std::unique_ptr<AliEventManager> m_eventMgr;
		std::unique_ptr<Renderpass> m_renderPass;
		std::unique_ptr<LinePipeline> m_linePipeline;
		std::unique_ptr<ModelPipeline> m_modelPipeline;

		vk::UniqueShaderModule m_lineVertexShader;
		vk::UniqueShaderModule m_lineControlShader;
		vk::UniqueShaderModule m_lineEvaluationShaderPush;
		vk::UniqueShaderModule m_lineEvaluationShaderUniform;
		vk::UniqueShaderModule m_lineFragmentShader;

		vk::UniqueShaderModule m_modelVertexShader;
		vk::UniqueShaderModule m_modelFragmentShader;

		std::vector<vk::UniqueCommandBuffer> m_commandBuffers;

		vk::UniqueDeviceMemory m_uniformBufferMem;
		vk::UniqueBuffer m_uniformBuffer;

		vk::UniqueDescriptorPool m_descriptorPool;
		vk::DescriptorSet m_descriptorSet;

		std::vector<std::unique_ptr<Track>> m_tracks;
		std::unique_ptr<Model> m_model;

		glm::mat4 m_view;
		glm::mat4 m_modelMatrix;

		Render::CalcType m_cType;
		Render::LineType m_lType;
		Render::RecordType m_rType;

		void loadTracks();

	public:
		RendererA(const Swapchain &swapchain, Render::CalcType cType, Render::LineType lType, Render::RecordType rType);
		virtual ~RendererA();

		virtual vk::CommandBuffer draw(unsigned long duration, uint32_t imageIndex);
		vk::CommandBuffer drawDynamic(uint32_t imageIndex, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
		void drawStatic(uint32_t imageIndex);

		virtual void prevEvent();
		virtual void nextEvent();

		virtual void initSwapchainDependent(const Swapchain &swapchain);
		virtual void freeSwapchainDependent();

		void updateStatic(glm::mat4 &VP);
};

#endif