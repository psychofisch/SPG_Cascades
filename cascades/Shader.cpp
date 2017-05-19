#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

Shader::Shader()
{
	m_program = glCreateProgram();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// If geometry shader is given, compile geometry shader
	GLuint geometry;
	if (geometryPath != nullptr)
	{
		const GLchar * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		checkCompileErrors(geometry, "GEOMETRY");
	}
	// Shader Program
	this->m_program = glCreateProgram();
	glAttachShader(this->m_program, vertex);
	glAttachShader(this->m_program, fragment);
	if (geometryPath != nullptr)
		glAttachShader(this->m_program, geometry);
	glLinkProgram(this->m_program);
	checkCompileErrors(this->m_program, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);

}

GLuint Shader::getGLProgramID()
{
	return m_program;
}

void Shader::AttachShaderToProgram(const char* path, int ShaderType)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string shaderCode;
	std::ifstream ShaderFile;
	// ensures ifstream objects can throw exceptions:
	ShaderFile.exceptions(std::ifstream::badbit);
	try
	{
		// Open files
		ShaderFile.open(path);
		std::stringstream ShaderStream;
		// Read file's buffer contents into streams
		ShaderStream << ShaderFile.rdbuf();
		// close file handlers
		ShaderFile.close();
		// Convert stream into string
		shaderCode = ShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar* ShaderCode = shaderCode.c_str();

	// 2. Compile shaders
	GLuint shader;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	shader = glCreateShader(ShaderType);
	glShaderSource(shader, 1, &ShaderCode, NULL);
	glCompileShader(shader);
	// Print compile errors if any
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		std::cin.ignore();
		exit(13);
	}
	glHandleError(__FILE__, __LINE__);

	// Shader Program
	//glUseProgram(m_program);
	glHandleError(__FILE__, __LINE__);
	glAttachShader(m_program, shader);

	if (ShaderType == GL_VERTEX_SHADER)
	{
		m_vs = shader;
	}
	else if (ShaderType == GL_GEOMETRY_SHADER)
	{
		m_gs = shader;
		const GLchar* feedbackVaryings[] = { "feedbackOut" };
		glTransformFeedbackVaryings(m_program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	}
	else if (ShaderType == GL_FRAGMENT_SHADER)
	{
		m_fs = shader;
	}

	//glLinkProgram(m_program);
	//// Print linking errors if any
	//glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	//if (!success)
	//{
	//	glGetProgramInfoLog(m_program, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	//	std::cin.ignore();
	//}

	//// Delete the shaders as they're linked into our program now and no longer necessary
	//glDeleteShader(shader);
	glHandleError(__FILE__, __LINE__);
}

void Shader::LinkShader()
{
	//return;
	GLint success;
	GLchar infoLog[512];

	//glUseProgram(m_program);
	glLinkProgram(m_program);
	// Print linking errors if any
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		//std::cin.ignore();
	}

	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(m_vs);
	glDeleteShader(m_gs);
	glDeleteShader(m_fs);

	m_vs = -1;
	m_gs = -1;
	m_fs = -1;

	glHandleError(__FILE__, __LINE__);
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}

	glHandleError(__FUNCTION__, __LINE__);
}

// Uses the current shader
void Shader::Use()
{
	glUseProgram(m_program);
	glHandleError(__FILE__, __LINE__);
}
