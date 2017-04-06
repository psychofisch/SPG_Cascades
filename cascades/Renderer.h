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
#include "ShaderManager.h"

void glHandleError(const char* info = "");

struct Sceneobj {
	GLuint VAO;
	GLsizei iCount;
	size_t shader;
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 origin;
	float scale;
	float dist;
	bool simple, triangles;
	GLuint texture;
	GLuint normal;

	bool operator < (const Sceneobj& t) const
	{
		return (dist < t.dist);
	}

	//void calcOrigin();
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	//callbacks
	void key_callback(int key, int action);
	void mouse_callback(double xpos, double ypos);

	void Run();

	GLFWwindow* createWindow(int width, int height);
	size_t addObjectToScene(GLfloat * vertices, int vSize, GLuint* vao);
	Sceneobj* getObjectById(size_t id);

	ShaderManager* getShaderManager();

private:
	GLFWwindow* m_window;
	glm::ivec2 m_size;
	Camera m_camera;
	glm::vec2 m_mouse;
	bool m_debug;
	std::vector<Sceneobj> m_scene;
	float m_dt;
	GLfloat m_nearPlane, m_farPlane;

	ShaderManager m_shaderManager;
	void i_renderScene(Sceneobj* Scene, size_t size);
};
