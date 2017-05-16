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

void ShaderManager::clearShader(size_t id)
{
	glDeleteShader(m_shaders[id]->getGLProgramID());
	delete m_shaders[id];
	m_shaders[id] = new Shader();
}

void ShaderManager::attachShaderToProgram(size_t Shader, const char* Path, int ShaderType)
{
	m_shaders[Shader]->AttachShaderToProgram(Path, ShaderType);
}

void ShaderManager::LinkShader(size_t shaderId)
{
	m_shaders[shaderId]->LinkShader();
}

void ShaderManager::UseShader(size_t id)
{
	m_shaders[id]->Use();
}

GLuint ShaderManager::getGLIdById(size_t id)
{
	return m_shaders[id]->getGLProgramID();
}
