#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

class ShaderManager
{
public:
	//enum Shaders{ SHADER_SIMPLE = 0, SHADER_TERRAIN, SHADER_SHADOW };

	ShaderManager();
	~ShaderManager();

	size_t createNewShader();
	void clearShader(size_t id);
	void attachShaderToProgram(size_t Shader, const char* Path, int ShaderType);
	void LinkShader(size_t shaderId);
	void UseShader(size_t id);
	GLuint getGLIdById(size_t id);

private:
	std::vector<Shader*> m_shaders;
};

