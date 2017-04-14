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

	TerrainCreator terrain(16, 16, 32);
	int t = time(NULL);
	terrain.createTerrain(t, 1);

	int vSize = terrain.getNumberOfVertices(false);
	GLfloat* vData = new GLfloat[vSize];
	terrain.getVertices(vData, false);

	GLuint vao;
	size_t objId = renderer.addObjectToScene(vData, vSize, &vao);

	ShaderManager* sM = renderer.getShaderManager();
	size_t mainShader = sM->createNewShader();
	sM->attachShaderToProgram(mainShader, "simple_vs.hlsl", GL_VERTEX_SHADER);
	sM->attachShaderToProgram(mainShader, "simple_fs.hlsl", GL_FRAGMENT_SHADER);
	sM->attachShaderToProgram(mainShader, "simple_gs.hlsl", GL_GEOMETRY_SHADER);

	renderer.getObjectById(objId)->shader = mainShader;
	renderer.getObjectById(objId)->iCount = vSize/3;

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
