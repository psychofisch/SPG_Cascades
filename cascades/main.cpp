#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Renderer.h"

Renderer* global_engine_ptr = nullptr;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void main(int agrc, char* argv[])
{
	Renderer renderer;
	global_engine_ptr = &renderer;
	GLFWwindow* window = renderer.createWindow(1280, 720);

	//bind keycallback method
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	GLfloat vertices[] = {
		-0.5f,  0.5f, // Top-left
		0.5f,  0.5f, // Top-right
		0.5f, -0.5f, // Bottom-right
		-0.5f, -0.5f  // Bottom-left
	};
	int verticesSize = 8;

	GLuint vao;
	renderer.addObject(vertices, verticesSize, &vao, true);

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
