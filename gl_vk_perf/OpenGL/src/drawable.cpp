#include "drawable.hpp"

#include <fstream>
#include <iostream>

gl::GLuint Drawable::loadCompileShader(gl::GLenum shaderType, std::string shaderFilename)
{
    std::fstream shaderFile;

	shaderFile.open("shaders/" + shaderFilename, std::ios::in);

    if(!shaderFile.is_open())
    {
        std::cerr << "Can't open shader file: " << shaderFilename << "." << std::endl;
        return 0;
    }

	std::string code, line;
	while(getline(shaderFile, line))
	{
		code += "\n" + line;
	}
	shaderFile.close();

	const gl::GLchar *bufPointer = code.c_str();

    gl::GLuint shader = gl::glCreateShader(shaderType);
    gl::glShaderSource(shader, 1, &bufPointer, nullptr);
    gl::glCompileShader(shader);

    gl::GLint error_status;

    gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &error_status);
    if(!error_status) {
        std::cerr << "Error compilling shader " << shaderFilename << std::endl;

		gl::GLint maxLength = 0;
        gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<gl::GLchar> infoLog(maxLength);
		gl::glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		std::cerr << infoLog.data() << std::endl;

        gl::glDeleteShader(shader);

        return 0;
    }

    return shader;
}

gl::GLuint Drawable::createProgram(std::vector<gl::GLuint> shaderVector)
{
    gl::GLuint program = gl::glCreateProgram();

    for(auto shaderObj : shaderVector)
        gl::glAttachShader(program, shaderObj);

    gl::glLinkProgram(program);

    gl::GLint error_status;

    gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &error_status);

    if (!error_status)
    {
        std::cerr << "Error linking program" << std::endl;

		gl::GLint maxLength = 0;
		gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<gl::GLchar> infoLog(maxLength);
		gl::glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		std::cerr << infoLog.data() << std::endl;

		gl::glDeleteProgram(program);
		return 0;
	}

	return program;
}
