#ifndef __H_RENDERERA__
#define __H_RENDERERA__

#include <memory>
#include <vector>

#include "../renderer.hpp"
#include "track.hpp"
#include "../common/alieventmanager.hpp"
#include "../common/enums.hpp"
#include "../../model.hpp"

class RendererA: public Renderer
{
	private:
		static const unsigned int PATCH_SIZE = 4;

		std::unique_ptr<AliEventManager> m_eventMgr;

		std::vector<std::unique_ptr<Track>> m_tracks;
		std::unique_ptr<Model> m_model;

		glm::mat4 m_view;

		Render::CalcType m_cType;
		Render::LineType m_lType;

		void loadTracks();

	public:
		RendererA(Render::CalcType cType, Render::LineType lType);
		virtual ~RendererA();

		virtual void draw(unsigned long duration);

		virtual void prevEvent();
		virtual void nextEvent();
};

#endif