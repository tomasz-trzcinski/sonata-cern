#ifndef __H_RENDERERD__
#define __H_RENDERERD__

#include <memory>

#include "../renderer.hpp"
#include "../common/enums.hpp"
#include "track_array_d.hpp"

class RendererD: public Renderer
{
	private:
		static const unsigned int PATCH_SIZE = 4;

		std::unique_ptr<TrackArrayD> m_tArray;

		glm::mat4 m_view;

	public:
		RendererD(Render::CalcType cType, Render::LineType lType);
		virtual ~RendererD();

		virtual void draw(unsigned long duration);

		virtual void prevEvent();
		virtual void nextEvent();
};

#endif