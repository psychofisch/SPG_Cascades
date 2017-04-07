#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h> // Include glew to get all the required OpenGL headers

class Shader
{
public:
	Shader();

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	GLuint getGLProgramID(size_t index);
	void AttachShaderToProgram(const char* path, int ShaderType);
	void Use();

private:
	GLuint m_program;
};
