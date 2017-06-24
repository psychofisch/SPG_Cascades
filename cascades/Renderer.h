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

#include "SOIL.h"

#include "glHandleError.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "TerrainCreator.h"
#include "vertTable.h"
#include "edgeTable.h"
#include "ParticleSystem.h"

void glHandleError(const char* info);
void glHandleError(const char* file, int line);

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

struct Light {
	Light()
		:position(0.0f),
		color(1.0f),
		intensity(1.0f)
	{};

	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float ambientIntensity;
};

struct Shadow {
	GLuint size;
	GLuint depthFBO;
	GLuint depthTex;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	//callbacks
	void key_callback(int key, int action);
	void mouse_callback(double xpos, double ypos);
	void mouse_button_callback(int button, int action);

	void Run();

	GLFWwindow* createWindow(int width, int height);
	size_t addObjectToScene(GLfloat * vertices, int vSize, GLuint* vao);
	Sceneobj* getObjectById(size_t id);
	void Renderer::setPerspective(float fovy, float aspect, float near, float far);
	void setTerrainCreatorPtr(TerrainCreator* tcPtr, GLuint textureId);

	ShaderManager* getShaderManager();

	//statics
	static float triangleHit(glm::vec3* triangle, glm::vec3 rOrigin, glm::vec3 rDirection, float length); //triangle: pointer to 3 glm::vec3

private:
	GLFWwindow* m_window;
	glm::ivec2 m_size;
	Camera m_camera;
	Light m_light;
	glm::vec2 m_mouse;
	bool m_debug,
		m_transformFeedbackSwitch,
		m_saveTransformFeedback;
	std::vector<Sceneobj> m_scene;
	float m_dt;
	GLfloat m_nearPlane, m_farPlane, m_densityThreshold;
	glm::mat4 m_projection;
	glm::mat4 m_view;
	TerrainCreator* m_terrainCreator;
	GLuint m_terrainTexture,
		m_vertTable,
		m_edgeTable,
		m_diffuseTexture,
		m_displacementTexture,
		m_framebuffer,
		m_rbo,
		m_drawMode,
		m_normalMap;
	ShaderManager m_shaderManager;
	ParticleSystem* m_particleSystem;
	Shadow m_shadow;
	bool m_particlesOn;
	int m_shadowMode,//0 - PCSS, 1 - PCF, 2 - Hard Shadows
		m_displaceMode;//0 - tess, 1 - mapping, 2 - off

	void i_renderScene(Sceneobj* Scene, size_t size);
	void i_renderArray(GLuint VAO, GLuint arraySize, int glDrawMode, size_t shaderManagerId);
	void i_generateNewFrameBuffer();
	void i_initShadow();
};
