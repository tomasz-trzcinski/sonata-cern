#ifndef __H_RENDERERT__
#define __H_RENDERERT__

#include <memory>

#include "../renderer.hpp"
#include "../common/enums.hpp"
#include "tst_line_draw.hpp"

class RendererTest: public Renderer
{
	private:
		static const unsigned int PATCH_SIZE = 4;

		std::unique_ptr<TestLineDraw> m_lineDraw;

		glm::mat4 m_view;

	public:
		RendererTest(Render::CalcType cType, Render::LineType lType);
		virtual ~RendererTest();

		virtual void draw(unsigned long duration);

		virtual void prevEvent() {};
		virtual void nextEvent() {};
};

#endif