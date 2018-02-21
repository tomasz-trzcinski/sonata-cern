#ifndef __H_RENDERERB__
#define __H_RENDERERB__

#include <memory>

#include "../renderer.hpp"
#include "../common/enums.hpp"
#include "track_array_b.hpp"

class RendererB: public Renderer
{
	private:
		static const unsigned int PATCH_SIZE = 4;

		std::unique_ptr<TrackArrayB> m_tArray;

		glm::mat4 m_view;

	public:
		RendererB(Render::CalcType cType, Render::LineType lType);
		virtual ~RendererB();

		virtual void draw(unsigned long duration);

		virtual void prevEvent();
		virtual void nextEvent();
};

#endif