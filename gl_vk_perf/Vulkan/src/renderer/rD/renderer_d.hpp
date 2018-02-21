#ifndef __H_RENDERERD__
#define __H_RENDERERD__

#include <memory>
#include <vector>

#include "../renderer.hpp"
#include "../../swapchain.hpp"
#include "../../renderpass.hpp"

#include "../common/alieventmanager.hpp"

#include "track_array_d.hpp"

#include "../../modelpipeline.hpp"
#include "../../model.hpp"

class RendererD : public Renderer
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

		std::unique_ptr<TrackArrayD> m_tArray;
		std::unique_ptr<Model> m_model;

		glm::mat4 m_view;

		Render::CalcType m_cType;
		Render::LineType m_lType;
		Render::RecordType m_rType;

	public:
		RendererD(const Swapchain &swapchain, Render::CalcType cType, Render::LineType lType, Render::RecordType rType);
		virtual ~RendererD();

		virtual vk::CommandBuffer draw(unsigned long duration, uint32_t imageIndex);
		vk::CommandBuffer drawDynamic(uint32_t imageIndex, unsigned long duration, glm::vec3 &cameraPos, glm::mat4 &VP);
		void drawStatic(uint32_t imageIndex);

		virtual void prevEvent();
		virtual void nextEvent();

		virtual void initSwapchainDependent(const Swapchain &swapchain);
		virtual void freeSwapchainDependent();
};

#endif