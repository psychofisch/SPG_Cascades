#pragma once

#include <climits>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"

struct Particle
{
	Particle()
		:lifetime(0.f)
	{}

	glm::vec3 position;
	GLfloat lifetime;
};

class ParticleSystem
{
public:
	ParticleSystem(GLuint maxParticles);
	~ParticleSystem();

	void update(float dt);
	void addEmitter(glm::vec3 position);
	GLuint getVAO();

private:
	GLuint m_activeEmitters,
		m_VAO,
		m_VBO,
		m_noOfParticles;
	Shader m_shader;
	GLfloat m_ttl;
	glm::vec3* m_emitterParticles;
	Particle* m_particles;
};

