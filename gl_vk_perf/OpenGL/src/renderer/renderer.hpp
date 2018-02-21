#ifndef __H_RENDERER__
#define __H_RENDERER__

#include <glm/matrix.hpp>

class Renderer
{
	protected:
		glm::mat4 m_projection;
	public:
		Renderer() {}
		virtual ~Renderer() {}

		virtual void draw(unsigned long duration) = 0;
		virtual void prevEvent() = 0;
		virtual void nextEvent() = 0;

		void handleResize(int width, int height);
};

#endif