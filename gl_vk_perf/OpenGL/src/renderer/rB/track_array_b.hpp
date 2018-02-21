#ifndef __H_TRACKARRAYB__
#define __H_TRACKARRAYB__

#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>

#include "../../drawable.hpp"
#include "../common/enums.hpp"
#include "../common/alieventmanager.hpp"

class TrackArrayB : public Drawable
{
	private:
		static const unsigned int ATTRIB_POS = 0;
		static const unsigned int POS_STREAM = 0;

		struct TrackInfo
		{
			gl::GLvoid* indexOffset;
			gl::GLint vertexOffset;
			gl::GLsizei count;
		};

		struct ParticleList
		{
			glm::vec3 color;
			std::vector<TrackInfo> trackInfos;
		};

		std::unique_ptr<AliEventManager> m_eventMgr;

		std::unordered_map<int32_t, ParticleList> m_renderList;

		gl::GLuint m_VAO;
		gl::GLuint m_vertexBuffer;
		gl::GLuint m_indexBuffer;

		gl::GLuint m_geomProgram;
		gl::GLuint m_hobbyProgram;
		gl::GLuint m_passProgram;

		glm::mat4 m_model;

		Render::CalcType m_cType;
		Render::LineType m_lType;

	private:
		void loadTracks();
	
	public:
		TrackArrayB(Render::CalcType cType, Render::LineType lType);
		virtual ~TrackArrayB();
	
		virtual void draw(unsigned long, glm::vec3&, glm::mat4&);
	
		void nextEvent();
		void prevEvent();
};

#endif
