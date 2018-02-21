#ifndef __H_TRACK__
#define __H_TRACK__

#include <memory>
#include <vector>
#include "../../drawable.hpp"
#include "../common/enums.hpp"

class Track : public Drawable
{
	private:
		static const unsigned int ATTRIB_POS = 0;

		static gl::GLuint s_geomProgram;
		static gl::GLuint s_hobbyProgram;
		static gl::GLuint s_passProgram;

		Render::CalcType m_cType;
		Render::LineType m_lType;

		gl::GLuint m_VAO;
		gl::GLuint m_vertexBuffer;
		gl::GLuint m_indexBuffer;
		gl::GLsizei m_count;

		glm::mat4 m_model;
		glm::vec3 m_color;

	public:
		Track(Render::CalcType cType, Render::LineType lType, const std::vector<glm::vec3> &pointList, glm::vec3 color);
		virtual ~Track();

		void draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP);

		static void createPrograms();
		static void destroyPrograms();
};

#endif