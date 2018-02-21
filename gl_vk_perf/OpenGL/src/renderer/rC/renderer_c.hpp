#ifndef __H_RENDERERC__
#define __H_RENDERERC__

#include <memory>

#include "../renderer.hpp"
#include "../common/enums.hpp"
#include "track_array_c.hpp"

class RendererC: public Renderer
{
	private:
		static const unsigned int PATCH_SIZE = 4;

		std::unique_ptr<TrackArrayC> m_tArray;

		glm::mat4 m_view;

	public:
		RendererC(Render::CalcType cType, Render::LineType lType);
		virtual ~RendererC();

		virtual void draw(unsigned long duration);

		virtual void prevEvent();
		virtual void nextEvent();
};

#endif