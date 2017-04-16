//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ctime>
#include <iostream>

#include "Renderer.h"
#include "TerrainCreator.h"

Renderer* global_engine_ptr = nullptr;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void main(int agrc, char* argv[])
{
	Renderer renderer;
	global_engine_ptr = &renderer;
	GLFWwindow* window = renderer.createWindow(1280, 720);

	//bind key callback method
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	renderer.setPerspective(70.0f, (GLfloat)1280 / (GLfloat)720, 0.1f, 500.0f);

	//GLfloat vData[] = {
	//	-0.5f,  0.5f, 0.f, // Top-left
	//	0.5f,  0.5f, 0.f,// Top-right
	//	0.5f, -0.5f, 0.f,// Bottom-right
	//	-0.5f, -0.5f, 0.f // Bottom-left
	//};
	//int vSize = 12;

	TerrainCreator::vec3i terrainSize(64, 64, 64);

	int t = time(NULL);
	TerrainCreator terrain(terrainSize.x, terrainSize.y, terrainSize.z, t);
	terrain.createTerrain();

	int vSize = terrain.getNumberOfVertices(true);
	GLfloat* vData = new GLfloat[vSize];
	terrain.getVertices(vData, true);

	GLuint vao;
	size_t objId = renderer.addObjectToScene(vData, vSize, &vao);

	GLuint densityTexture;
	glGenTextures(1, &densityTexture);
	glBindTexture(GL_TEXTURE_3D, densityTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, terrainSize.x, terrainSize.y, terrainSize.z, 0, GL_RED, GL_FLOAT, terrain.getTerrainData());
	glHandleError("post texture");
	glBindTexture(GL_TEXTURE_3D, 0);

	renderer.setTerrainCreatorPtr(&terrain, densityTexture);

	ShaderManager* sM = renderer.getShaderManager();
	size_t mainShader = sM->createNewShader();
	sM->attachShaderToProgram(mainShader, "simple_vs.hlsl", GL_VERTEX_SHADER);
	sM->attachShaderToProgram(mainShader, "simple_fs.hlsl", GL_FRAGMENT_SHADER);
	sM->attachShaderToProgram(mainShader, "simple_gs.hlsl", GL_GEOMETRY_SHADER);

	renderer.getObjectById(objId)->shader = mainShader;
	renderer.getObjectById(objId)->iCount = vSize / 3;
	renderer.getObjectById(objId)->texture = densityTexture;

	renderer.Run();
	
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (global_engine_ptr == nullptr)
	{
		std::cout << "no engine pointer found\n";
	}
	else
		global_engine_ptr->key_callback(key, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	global_engine_ptr->mouse_callback(xpos, ypos);
}
