#pragma once

#include <climits>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"

//Particle DataStructure
//	float x, y, z, vx, vy, vz, lifetime -> sizeof(GLfloat) * 4
#define DataInParticle 7
#define SizeOfParticle (sizeof(GLfloat) * DataInParticle)
//
//Emitter DataStructure
//	float x, y, z, normalX, normalY, normalZ
#define DataInEmitter 6
#define SizeOfEmitter (sizeof(GLfloat) * DataInEmitter)

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(GLuint maxParticles, GLuint maxEmitters);
	~ParticleSystem();

	void update(float dt);
	void draw(glm::mat4 projection, glm::mat4 view);
	void addEmitter(glm::vec3 position, glm::vec3 normal);
	GLuint getVAO();

private:
	GLuint m_activeEmitters,
		m_VAO,
		m_VBO,
		m_maxParticles,
		m_feedbackBuffer,
		m_maxEmitters;
	Shader m_computeShader,
		m_drawShader;
	GLfloat m_maxLifetime;
	GLfloat* m_emitters;
	//Particle* m_particles;
	GLfloat* m_particles;
};

