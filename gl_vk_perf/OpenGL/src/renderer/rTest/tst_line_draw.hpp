#ifndef __H_TSTLINEDRAW__
#define __H_TSTLINEDRAW__

#include <memory>
#include <tuple>
#include <vector>
#include <unordered_map>

#include "../../drawable.hpp"
#include "../common/enums.hpp"

class TestLineDraw : public Drawable
{
	private:
		static const unsigned int ATTRIB_POS = 0;
		static const unsigned int POS_STREAM = 0;
		static const unsigned int PATCH_SIZE = 4;

		std::vector<gl::GLsizei> counts;
		std::vector<gl::GLvoid*> indices;
		std::vector<gl::GLint> bases;

		gl::GLuint VAO;
		gl::GLuint vertexBuffer;
		gl::GLuint indexBuffer;

		gl::GLuint genericProgram;
		gl::GLuint bezierProgram;
		gl::GLuint hobbyProgram;
		gl::GLuint passProgram;

		Render::CalcType m_cType;
		Render::LineType m_lType;

		gl::GLint m_startOffset;

		glm::mat4 Model;

	private:
		void setupData();
	
	public:
		TestLineDraw(Render::CalcType cType, Render::LineType lType);
		virtual ~TestLineDraw();
	
		void draw(unsigned long duration, glm::vec3 &camPos, glm::mat4 &VP);
};

#endif
