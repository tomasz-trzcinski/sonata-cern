#ifndef __H_DRAWABLE__
#define __H_DRAWABLE__

#include <glbinding/gl40core/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Drawable
{
    protected:
        static gl::GLuint loadCompileShader(gl::GLenum, std::string);
        static gl::GLuint createProgram(std::vector<gl::GLuint>);

    public:
        virtual ~Drawable() {};
        virtual void draw(unsigned long, glm::vec3&, glm::mat4&) = 0;

};

#endif
