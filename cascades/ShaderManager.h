#pragma once

#include <vector>
#include "Shader.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	size_t createNewShader();
	void attachShaderToProgram(size_t Shader, const char* Path, int ShaderType);
	void UseShader(size_t id);

private:
	std::vector<Shader*> m_shaders;
};

