#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

void glHandleError(const char* info = "");

class Renderer
{
public:
	Renderer();
	~Renderer();

	//callbacks
	void key_callback(int key, int action);
	void mouse_callback(double xpos, double ypos);

	void Run();

	GLFWwindow* createWindow(int widht, int height);

	float dt, fps;

private:
	GLFWwindow* m_window;
	glm::ivec2 m_size;
	Camera m_camera;
	glm::vec2 m_mouse;
	bool m_debug;
};
