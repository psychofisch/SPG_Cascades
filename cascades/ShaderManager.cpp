#include "ShaderManager.h"



ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
}

size_t ShaderManager::createNewShader()
{
	size_t result = m_shaders.size();
	m_shaders.push_back(new Shader());
	return result;
}

void ShaderManager::attachShaderToProgram(size_t Shader, const char* Path, int ShaderType)
{
	m_shaders[Shader]->AttachShaderToProgram(Path, ShaderType);
}

void ShaderManager::UseShader(size_t id)
{
	m_shaders[id]->Use();
}
