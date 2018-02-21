#ifndef __H_MODEL__
#define __H_MODEL__

#include "drawable.hpp"
#include "../common/alimodelmanager.hpp"

#include <memory>

class Model : public Drawable
{
    private:
        static const unsigned int ATTRIB_POS = 0;

		std::unique_ptr<AliModelManager> m_modelMgr;

        gl::GLuint m_VAO;
		gl::GLuint m_vertexBuffer;
		gl::GLuint m_indexBuffer;

        gl::GLuint m_program;

        glm::mat4 m_model;

		std::vector<size_t> m_indexOffsets;
		std::vector<gl::GLsizei> m_drawSizes;
		gl::GLsizei m_elements;

    public:
        Model();
        virtual ~Model();

        virtual void draw(unsigned long, glm::vec3&, glm::mat4&);

};

#endif
