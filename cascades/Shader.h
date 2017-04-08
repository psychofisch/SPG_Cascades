#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	Shader();

	//Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);
	GLuint getGLProgramID();
	void AttachShaderToProgram(const char* path, int ShaderType);
	void Use();

private:
	GLuint m_program;

	void checkCompileErrors(GLuint shader, std::string type);
};
