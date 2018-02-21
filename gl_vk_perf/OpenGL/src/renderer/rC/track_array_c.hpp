#ifndef __H_TRACKARRAYC__
#define __H_TRACKARRAYC__

#include <memory>
#include <vector>

#include "../../drawable.hpp"
#include "../common/enums.hpp"
#include "../common/alieventmanager.hpp"

class TrackArrayC : public Drawable
{
	private:
		static const unsigned int ATTRIB_POS = 0;

		std::unique_ptr<AliEventManager> m_eventMgr;

		std::vector<gl::GLvoid*> m_indexOffsets;
		std::vector<gl::GLint> m_vertexOffsets;
		std::vector<gl::GLsizei> m_counts;

		gl::GLuint m_VAO;
		gl::GLuint m_vertexBuffer;
		gl::GLuint m_indexBuffer;
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
		TrackArrayC(Render::CalcType cType, Render::LineType lType);
		virtual ~TrackArrayC();
	
		virtual void draw(unsigned long, glm::vec3&, glm::mat4&);
	
		void nextEvent();
		void prevEvent();
};

#endif
