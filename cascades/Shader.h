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
	void LinkShader();
	void Use();

private:
	GLuint m_program,
		m_vs, m_gs, m_fs;

	void checkCompileErrors(GLuint shader, std::string type);
};
