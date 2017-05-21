#pragma once

#include <climits>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"

//struct Particle
//{
//	Particle()
//		:lifetime(666.f)
//	{}
//
//	glm::vec3 position;
//	GLfloat lifetime;
//};

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(GLuint maxParticles);
	~ParticleSystem();

	void update(float dt);
	void draw(glm::mat4 projection, glm::mat4 view);
	void addEmitter(GLfloat x, GLfloat y, GLfloat z);
	GLuint getVAO();

private:
	GLuint m_activeEmitters,
		m_VAO,
		m_VBO,
		m_noOfParticles,
		m_feedbackBuffer;
	Shader m_computeShader,
		m_drawShader;
	GLfloat m_maxLifetime;
	glm::vec3* m_emitterParticles;
	//Particle* m_particles;
	GLfloat* m_particles;
};

