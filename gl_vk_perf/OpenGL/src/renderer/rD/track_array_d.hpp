#ifndef __H_TRACKARRAYD__
#define __H_TRACKARRAYD__

#include <memory>
#include <vector>

#include "../../drawable.hpp"
#include "../common/enums.hpp"
#include "../common/alieventmanager.hpp"

class TrackArrayD : public Drawable
{
	private:
		static const unsigned int ATTRIB_POS = 0;

		std::unique_ptr<AliEventManager> m_eventMgr;

		struct DrawCmd
		{
			gl::GLuint vertexCount;
			gl::GLuint instanceCount;
			gl::GLuint firstIndex;
			gl::GLint baseVertex;
			gl::GLuint baseInstance;

			DrawCmd() : vertexCount(0), instanceCount(1), firstIndex(0), baseVertex(0), baseInstance(0) {};
		};

		gl::GLsizei m_count;

		gl::GLuint m_VAO;
		gl::GLuint m_vertexBuffer;
		gl::GLuint m_indexBuffer;
		gl::GLuint m_commandBuffer;
		gl::GLuint m_storageBuffer;

		gl::GLuint m_geomProgram;
		gl::GLuint m_hobbyProgram;
		gl::GLuint m_passProgram;

		glm::mat4 m_model;

		Render::CalcType m_cType;
		Render::LineType m_lType;

	private:
		void loadTracks();
	
	public:
		TrackArrayD(Render::CalcType cType, Render::LineType lType);
		virtual ~TrackArrayD();
	
		virtual void draw(unsigned long, glm::vec3&, glm::mat4&);
	
		void nextEvent();
		void prevEvent();
};

#endif
